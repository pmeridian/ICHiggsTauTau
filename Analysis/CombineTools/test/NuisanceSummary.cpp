#include <string>
#include <vector>
#include "CombineTools/interface/CombineHarvester.h"
#include "CombineTools/interface/Observation.h"
#include "CombineTools/interface/Process.h"
#include "CombineTools/interface/Nuisance.h"
#include "CombineTools/interface/HelperFunctions.h"
#include "RooFitResult.h"
#include "boost/format.hpp"
#include "boost/program_options.hpp"

namespace po = boost::program_options;
using std::vector;
using std::string;

int main(int argc, char* argv[]) {
  vector<string> datacards;
  string fitresult_file = "";
  string parse_rule = "";

  po::options_description config("configuration");
  po::variables_map vm;
  po::notify(vm);
  config.add_options()
    ("datacard,d",
      po::value<vector<string>> (&datacards)->multitoken()->required())
    ("fitresult,f",
      po::value<string> (&fitresult_file))
    ("parse_rule,p",
      po::value<string> (&parse_rule)
      ->default_value("{MASS}/{ANALYSIS}_{CHANNEL}_{BINID}_{ERA}.txt"));

  po::store(po::command_line_parser(argc, argv).
      options(config).run(), vm);
  po::notify(vm);

  ch::CombineHarvester cmb;
  for (auto const& d : datacards) {
    cmb.ParseDatacard(d, parse_rule);
  }

  cmb.ForEachNus(ch::SetStandardBinName<ch::Nuisance>);
  cmb.ForEachObs(ch::SetStandardBinName<ch::Observation>);
  cmb.ForEachProc(ch::SetStandardBinName<ch::Process>);
  // cmb.PrintAll();

  if (fitresult_file.length()) {
    RooFitResult *fitresult = nullptr;
    fitresult = new RooFitResult(
        ch::OpenFromTFile<RooFitResult>(fitresult_file));
    auto fitparams = ch::ExtractFitParameters(*fitresult);
    cmb.UpdateParameters(fitparams);
    delete fitresult;
  }

  auto systematics = cmb.GenerateSetFromNus<std::string>(
      std::mem_fn(&ch::Nuisance::name));
  std::set<string> ungrouped;


  vector< std::pair<string, vector<string>> > groups  = {
    {"TT Norm"              , {"ttbarNorm"}},
    {"Bin-by-Bin"           , {"_bin_"}},
    {"B-Tagging Eff"        , {"eff_b"}},
    {"Electron Eff"         , {"eff_e"}},
    {"Muon Eff"             , {"eff_m"}},
    {"Diboson Norm"         , {"DiBosonNorm"}},
    {"Fakes Norm"           , {"fakes_em"}},
    {"MET Scale"            , {"scale_met"}},
    {"Z->LL Norm"           , {"zttNorm"}},
    {"Elec Energy Scale"    , {"scale_e"}},
    {"Jet Energy Scale"     , {"scale_j"}},
    {"QCD Scale"            , {"QCDscale"}},
    {"UEPS"                 , {"UEPS"}},
    {"Luminosity(7TeV)"     , {"lumi_7TeV"}},
    {"Luminosity(8TeV)"     , {"lumi_8TeV"}},
    {"PDF(qq)"              , {"pdf_qqbar"}},
    {"PDF(gg)"              , {"pdf_gg"}},
    {"Tau Eff"              , {"eff_t"}},
    {"QCD Shape"            , {"QCDShape"}},
    {"QCD Norm"             , {"QCDSyst"}},
    {"W Norm"               , {"WNorm"}},
    {"Jet->Tau Fake Rate"   , {"ZJetFakeTau"}},
    {"Z->ll Scale"          , {"ZLScale"}},
    {"Elec->Tau Fake Rate"  , {"ZLeptonFakeTau_etau"}},
    {"Muon->Tau Fake Rate"  , {"ZLeptonFakeTau_mutau"}},
    {"Tau Energy Scale"     , {"scale_t"}},
    {"Fakes Shape"          , {"FakeShape_em"}},
    {"Z->tt Extrapolation"  , {"extrap_ztt"}}
  };

  std::map<string, vector<string>> group_map;
  for (auto const& g : groups) {
    if (!group_map.count(g.first)) group_map[g.first] = vector<string>();
  }

  // Loop through set of systematic names
  for (auto const& s : systematics) {
    bool grouped = false;   // not grouped by default
    // loop through each grouping
    for (auto const& g : groups) {
      // loop through the patterns for this group
      for (auto const& gsub : g.second) {
        // if pattern is found add the nuisance to the list for this group
        if (s.find(gsub) != string::npos) {
          group_map[g.first].push_back(s);
          grouped = true;
          break;  // can skip other patterns
        }
      }
    }
    if (!grouped) ungrouped.insert(s);
  }
  for (auto const& s : ungrouped) {
    std::cout << "Ungrouped: " << s << "\n";
    if (!group_map.count(s)) group_map[s] = {s};
  }

  for (auto const& p : group_map) {
    if (p.second.size() == 0) continue;
    std::cout << "Group: " << p.first << "\n";
    std::cout << "  * Matched nuisance parameters:\n";
    for (auto const& n : p.second) std::cout << "      " << n << "\n";
    std::cout << "  * Affecting channels:\n      ";
    std::set<double> cleaned_rates;
    ch::CombineHarvester all_syst = cmb.shallow_copy()
      .nus_name(true, p.second);
    auto chns = all_syst
      .GenerateSetFromNus<string>(std::mem_fn(&ch::Nuisance::channel));
    for (auto const& c : chns) std::cout << " " << c << " ";
    std::cout << "\n";
    std::set<string> all_procs;
    auto bins = all_syst
      .GenerateSetFromNus<string>(std::mem_fn(&ch::Nuisance::bin));
    for (auto const& bin : bins) {
      ch::CombineHarvester syst_in_bin = all_syst.shallow_copy()
        .bin(true, {bin});
      auto procs = syst_in_bin
        .GenerateSetFromNus<string>(std::mem_fn(&ch::Nuisance::process));
      for (auto const& proc : procs) {
        all_procs.insert(proc);
        ch::CombineHarvester syst_in_proc = syst_in_bin.shallow_copy()
          .process(true, {proc});
        // syst_in_proc.PrintAll();
        double uncert = syst_in_proc.GetUncertainty()/syst_in_proc.GetRate();
        if (uncert == uncert) cleaned_rates.insert(uncert*100.);
      }
    }
    std::cout << "  * Affecting processes:\n      ";
    for (auto const& a : all_procs) std::cout << " " << a << " ";
    std::cout << "\n";
    std::cout << "  * Uncertainty range: ";
    std::cout << (boost::format("%.0f") % *(cleaned_rates.begin()))
      << "% - " << (boost::format("%.0f") % *(--cleaned_rates.end()))
      << "%" << std::endl;
  }
}