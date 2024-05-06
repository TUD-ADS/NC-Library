#!/bin/bash

pushd ../build/test || exit 1

all_good=0
for tst in "$@";
    do echo "---------- $tst -----------"
    ./"$tst" > test_"$tst"_run.txt 2> test_"$tst"_err.txt
    i=$?
    if [ $i -ne 0 ]; then
        ((all_good=all_good+1))
        echo Test failed with:
        cat test_"$tst"_err.txt
    else
        echo Test passed.
    fi
done

echo =================================
if [ $all_good -eq 0 ]; then
    echo All test passed!
else
    echo $all_good test failed.
fi

popd || exit 1
