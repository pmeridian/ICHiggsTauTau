Introduction {#mainpage}
============

This page contains documentation for the **ICHiggsTauTau** CMSSW package. This provides:
 
 * A C++ namespace, **ic**, under which all new classes, structs and functions should be defined
 * A set of physics object classes, e.g. ic::Electron and ic::Jet which can be stored in ntuples and manipulated in analysis code. The definition and implementation of these classes can be found in the interface and src directories of this package. These classes and associated ROOT I/O dictionaries are generated by the `scram` tool
 * CMSSW producer plugins that manufactures these physics objects from CMS datasets, located in the plugins directory of the package. These modules are also compiled by `scram`.
 * An offline analysis framework which is mostly independent of the CMSSW framework. This is organised into a series of packages located in the `Analysis` directory. This area is intended to contain all the code for event processing, applying cut-flows and making final plots. 

 The tree on the left of this page can be used to navigate around the documentation. Most has been automatically generated by doxygen, and can be accessed via *Namespaces*, *Data Structures* or *Files*.

Getting Started
===============
 	* \ref cmssw-and-git \n Follow these instructions for setting up a new CMSSW area using `git` and adding the **ICHiggsTauTau** package
  * \ref build-system \n Read about the folder structure and the build system for the analysis framework part of the **ICHiggsTauTau** package 
  * Sign-up for commit notification emails by joining the *ic-cms-github-notifications* e-group [here](https://e-groups.cern.ch/e-groups) (CERN login required). Notifications are sent whenever new content (commits, branches, tags etc.) are pushed to our CMSSW or analysis repositories.


Adding your own documentation
==============================

...coming soon...