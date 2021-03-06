Cookbook {#cookbookpage}
========

Two pipelines to call iSNVs from known and unknown reference sequences have been written in [snakemake](https://snakemake.readthedocs.io/en/stable/).

The two pipelines are distributed along with iVar and are present in the[ pipeline/](https://github.com/andersen-lab/ivar/tree/master/pipeline) and [pipeline_field/](https://github.com/andersen-lab/ivar/tree/master/pipeline_field) fodlers respectively.

For both pipelines, there are four parameters that will have to be set in beginning of the Snakefile.

```
in_dir = "<input fastq files>"
out_dir = "<output directory>"
bed = "<bed-file-with-primer-positions>"
ref="<path to reference fasta>"
```

### Call iSNVs from samples with known reference sequence

[Link to pipeline](https://github.com/andersen-lab/ivar/tree/master/pipeline)

#### Dependencies:

* [iVar](https://github.com/andersen-lab/ivar)
* [samtools](https://htslib.org/)
* [bwa](https://github.com/lh3/bwa)


<img src="https://raw.githubusercontent.com/gkarthik/IVAR/master/pipeline/pipeline.png" width="500" />

### Call iSNVs from samples with unknown reference sequence

[Link to pipeline](https://github.com/andersen-lab/ivar/tree/master/pipeline_field/)

#### Dependencies:

* [iVar](https://github.com/andersen-lab/ivar)
* [samtools](https://htslib.org/)
* [bwa](https://github.com/lh3/bwa)
* [bedtools](https://bedtools.readthedocs.io/en/latest/)

<img src="https://raw.githubusercontent.com/gkarthik/IVAR/master/pipeline_field/pipeline_field.png" width="500" />
