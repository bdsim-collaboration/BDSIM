#!/bin/bash

# submit jobs -
# Example : submit_jobs ebds_job.gmad 10 1000


usage="Usage : submit_jobs [gmad template file] [njobs] [nevents] \n "

qsub=$(which qsub 2> /dev/null)
qopt="-q medium"

echo submitting $2 jobs with $3 events each based on file $1

if [[ ! -e $1 ]]; then 
echo "ERROR: file $1 doesn't exist"
exit 0
fi

if [[ $2 < 1 ]]; then
echo "ERROR: number of jobs should be greater than 0"
exit 0
fi

if [[ $3 < 1 ]]; then
echo "ERROR: number of events per job should be greater than 0"
exit 0
fi


for((i=0;i<$2;i++)); do
  inpfilename=$1.$i
  outfilename="output"$i
  jobfilename="job.sh".$i
  offset=$3*$i
  echo submitting job $inpfilename
  cat $1 | sed "
           s/@NGENERATE/$3/g;
           s/@NIGNORE/$offset/g;
               " > $inpfilename;
  cat "job.sh" | sed "
           s/@INF/$inpfilename/g;
           s/@OUTF/$outfilename/g;
               " > $jobfilename;
#  $qsub $qopt $jobfilename 
done

 
