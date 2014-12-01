#!/bin/sh

outdir=$1
shift

walkdeps()
{
    echo $3
    cat $3/SOURCES 2>/dev/null | sed -e "s:^..*:$3/&:" | xargs -r dirname
    for dep in $([ -f $3/DEPS ] && gcc -E -P -x c -DASM_FILE -DTARGET_$(echo $1 | tr / _) -DTARGET=$1 -DCONFIG_H=target/$1/config.h -DTARGET_H=target/$1/target.h -Isrc -D$2 $3/DEPS | grep -v '^\#' | sed -e "s:^..*:$3/&:")
    do
        walkdeps $1 $2 $dep
    done
}

mkdir -p $outdir
echo "[CP]     Makefile"
cp Makefile $outdir/
echo "[GEN]    TARGETS"
rm -f $outdir/TARGETS
make clean
(
    echo src/.
    for target in $*
    do
        echo $target >> $outdir/TARGETS
        for build in debug release
        do
            walkdeps $target $(echo $build | tr '[a-z]' '[A-Z]') src
            make -k TARGET=$target TYPE=$build >&2
        done
    done
    find build | grep -E '.dep$' | xargs cat | tr '\\:\n' ' ' | tr -s ' ' '\n' | xargs -r dirname
) | grep '^src/' | sort | uniq | xargs -n 1 readlink -e | sort | uniq | sed -e "s:$(pwd)/\\(.*\\):\\1:" | while read dir
do
    echo "[CP]     $dir"
    mkdir -p $outdir/$dir
    cp $dir/* $outdir/$dir/ 2>/dev/null
done

