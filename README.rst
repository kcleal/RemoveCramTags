RemoveCramTags
==============

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

Otherwise set LDFLAGS and CPPFLAGS, e.g::

    CPPFLAGS="-I/htslib/include" LDFLAGS+="-L/htslib/lib" make

Usage
-----
Run using::

    RemoveCramTags DThreads CThreads REF INPUT OUTPUT TAGS
        DThreads - decompression threads
        CThreads - compression threads
        REF      - reference fasta file
        INPUT    - Input SAM/BAM/CRAM
        OUTPUT   - Output SAM/BAM/CRAM
        TAGS     - List of tags, comma separated

Example usage::

    RemoveCramTags 4 2 $HG19 in.bam out.bam BI,BD

    RemoveCramTags 4 2 $HG19 in.bam out.cram BI

