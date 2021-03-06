#!/bin/sh

## Try and take the JOBWRAPPER and JOBSUBMIT commands
## from the environment if set, otherwise use these defaults
: ${JOBWRAPPER:="./scripts/generate_job.sh"}
: ${JOBSUBMIT:="eval"}

export JOBSUBMIT="./scripts/submit_ic_batch_job.sh hepmedium.q"


echo "Using job-wrapper: " $JOBWRAPPER
echo "Using job-submission: " $JOBSUBMIT

CONFIG=scripts/DefaultConfigSkimMC.cfg
echo "Config file: $CONFIG"
PRODUCTION=Apr04


for CHANNEL in enu munu taunu
  do

  JOBDIR=jobs/skim/$CHANNEL/
  OUTDIR=/vols/ssd00/cms/invskims/$CHANNEL/$PRODUCTION/
  mkdir -p $JOBDIR
  mkdir -p $OUTDIR


#Process bkg common with HiggsTautau
  PREFIX=root://xrootd.grid.hep.ph.ic.ac.uk//store/user/pdunne/$PRODUCTION/MCtaushared/
  for FILELIST in `ls filelists/$PRODUCTION/medium/${PRODUCTION}_MCtaushared_W*ToLNu*`
    do
    echo "Processing files in "$FILELIST
    
    echo $FILELIST > tmp.txt
    sed "s/filelists\/${PRODUCTION}\/medium\/${PRODUCTION}_MCtaushared_//" tmp.txt > tmp2.txt    
    LOOPOUTDIR=$OUTDIR/`sed "s/\.dat//" tmp2.txt`/
    JOB=MC_`sed "s/\.dat//" tmp2.txt`

    mkdir -p $LOOPOUTDIR
    
    rm tmp.txt tmp2.txt
    
    echo "JOB name = $JOB"
    echo "OUTPUT dir = $LOOPOUTDIR"
    $JOBWRAPPER "./bin/HiggsNuNu --cfg=$CONFIG --filelist="$FILELIST" --input_prefix=$PREFIX --output_name=$JOB.root --skim_path=$LOOPOUTDIR --channel=$CHANNEL &> $JOBDIR/$JOB.log" $JOBDIR/$JOB.sh
    $JOBSUBMIT $JOBDIR/$JOB.sh
    
  done

done


#if (( "$#" != "2" ))
#then
#echo ""
#    exit
#fi

