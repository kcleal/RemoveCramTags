RemoveCramTags
==============

Note, this is not usable yet!

What is this?
-------------

A program to quickly remove tag(s) from SAM/BAM/CRAM files

Install
-------
RemoveCramTags relies on htslib. If you are using conda::

    export CONDA_PREFIX=/path/to/miniconda3
    make

If you have htslib in a directory somewhere::

    export HTSLIB=/path/to/htslib
    make

If you are using conda-build::

    conda-build .
    # change this to correct path:
    conda install miniconda3/conda-bld/linux-64/remove-cram-tags-0.0.1-h2bc3f7f_0.tar.bz2

Otherwise set LDFLAGS and CPPFLAGS, e.g::

    CPPFLAGS="-I/htslib" LDFLAGS+="-L/htslib" make

Usage
-----
Run using::

    Usage: RemoveCramTags [ARGS]

Positional arguments::

    RemoveCramTags DThreads CThreads REF INPUT OUTPUT TAGS
        DThreads - Decompression threads
        CThreads - Compression threads
        REF      - Reference fasta file
        INPUT    - Input SAM/BAM/CRAM
        OUTPUT   - Output SAM/BAM/CRAM
        TAGS     - List of tags, comma separated

Example usage::

    RemoveCramTags 4 2 $HG19 in.bam out.bam BI,BD

    RemoveCramTags 4 2 $HG19 in.bam out.cram BI

    RemoveCramTags 4 2 $HG19 - - BI   # stdin to stdout; output format will be cram

