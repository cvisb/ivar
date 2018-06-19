#include <iostream>
#include <unistd.h>

#include "remove_reads_from_amplicon.h"
#include "call_consensus_pileup.h"
#include "call_variants.h"
#include "trim_primer_quality.h"

struct args_t {
  std::string bam;		// -i
  std::string bed;		// -b
  std::string prefix;		// -p
  std::string ref;		// -r
  std::string region;		// -R
  uint8_t min_qual;		// -q
  uint8_t sliding_window;
} g_args;

void print_usage(){
  std::cout <<
    "Usage:	ivar [command <trim|callvariants|filtervariants|consensus|createbed|removereads|getmasked>]\n"
    "\n"
    "        Command       Description\n"
    "           trim       Trim reads in aligned bams\n"
    "       variants       Call Variants from aligned bam\n"
    " filtervariants       Filter variants across replicates\n"
    "      consensus       Call consensus from vcf file\n"
    "      createbed       Created bed file from primer sequences\n"
    "    removereads       Remove reads from aligned bam\n"
    "      getmasked       Get amplicons with primer mismatches\n"
    "\n"
    "To view detailed usage for each command type `ivar <command>` \n";
}

void print_trim_usage(){
  std::cout <<
    "Usage: ivar trim [-i input] [-b <bedfile>] [-p prefix]\n\n"
    "Input Options    Description\n"
    "           -i    (Required) Input, aligned bam file to trim primers and quality\n"
    "           -b    (Required) Bed File, Bed file with primer sequences and positions.\n"
    "           -R    Region in BAM\n"
    "           -q    Minimum quality threshold for sliding window to pass (Default: 20)\n"
    "           -s    Width of sliding window (Default: 4)\n\n"
    "Output Options   Description\n"
    "           -p    (Required) Prefix, prefix for the output file\n";
}

void print_variants_usage(){
  std::cout <<
    "Usage: samtools mpileup -A -d 300000 -A -Q 0 -F 0 -i <input-bam> | ivar variants [-p prefix] [-r <reference-fasta>]\n\n"
    "Note : samtools mpileup output must be piped into ivar variants\n\n"
    "Input Options    Description\n"
    "           -r    (Required) Reference fasta file\n"
    "           -q    Minimum quality threshold to count base (Default: 20)\n"
    "Output Options   Description\n"
    "           -p    (Required) Prefix, prefix for the output tsv file\n";
}

void print_consensus_usage(){
  std::cout <<
    "Usage: samtools mpileup -A -d 300000 -A -Q 0 -F 0 -i [<input-bam>] | ivar consensus [-p prefix] \n\n"
    "Note : samtools mpileup output must be piped into `ivar consensus`\n\n"
    "Input Options    Description\n"
    "           -q    Minimum quality threshold to count base (Default: 20)\n"
    "Output Options   Description\n"
    "           -p    (Required) Prefix, prefix for the output tsv file\n";
}


void print_removereads_usage(){
  std::cout <<
    "Usage: ivar removereads [-i <input-bam>] [-p prefix] primer-index-1 primer-index-2 primer-index-3 primer-index-4 ...  \n\n"
    "Input Options    Description\n"
    "           -i    (Required) Input BAM file run through `ivar trim` command whcih adds the primer number to BAM auxillary data\n"
    "Output Options   Description\n"
    "           -p    (Required) Prefix, prefix for the filtered BAM file\n";
}

static const char *trim_opt_str = "i:b:p:R::q::s::h?";
static const char *variants_opt_str = "p:r:q::h?";
static const char *consensus_opt_str = "p:q::h?";
static const char *removereads_opt_str = "i:p:h?";

int main(int argc, char* argv[]){
  if(argc == 1){
    print_usage();
    return -1;
  }
  std::string cmd(argv[1]);
  int opt = 0, res = 0;
  // Sift arg by 1 for getopt
  argv[1] = argv[0];
  argv++;
  argc--;
  g_args.min_qual = 255;
  g_args.sliding_window = 255;
  if (cmd.compare("trim") == 0){
    opt = getopt( argc, argv, trim_opt_str);
    while( opt != -1 ) {
      switch( opt ) {
      case 'i':
	g_args.bam = optarg;
	break;
      case 'b':
	g_args.bed = optarg;
	break;
      case 'p':
	g_args.prefix = optarg;
	break;
      case 'R':
	g_args.region = optarg;
	break;
      case 'q':
	g_args.min_qual = atoi(optarg);
	break;
      case 's':
	g_args.sliding_window = atoi(optarg);
	break;
      case 'h':
      case '?':
	print_trim_usage();
	return -1;
	break;
      }
      opt = getopt( argc, argv, trim_opt_str);
    }
    if(g_args.bam.empty() || g_args.bed.empty() || g_args.prefix.empty()){
      print_trim_usage();
      return -1;
    }
    res = trim_bam_qual_primer(g_args.bam, g_args.bed, g_args.prefix, g_args.region);
  } else if (cmd.compare("variants") == 0){
    opt = getopt( argc, argv, variants_opt_str);
    while( opt != -1 ) {
      switch( opt ) {
      case 'p':
	g_args.prefix = optarg;
	break;
      case 'r':
	g_args.ref = optarg;
	break;
      case 'q':
	g_args.min_qual = atoi(optarg);
	break;
      case 'h':
      case '?':
	print_variants_usage();
	return 0;
	break;
      }
      opt = getopt( argc, argv, variants_opt_str);
    }
    if(g_args.prefix.empty() || g_args.ref.empty()){
      print_variants_usage();
      return -1;
    }
    g_args.min_qual = (g_args.min_qual == 255) ? 20 : g_args.min_qual;
    res = call_variants_from_plup(std::cin, g_args.prefix, g_args.min_qual);
  } else if (cmd.compare("consensus") == 0){
    opt = getopt( argc, argv, consensus_opt_str);
    while( opt != -1 ) {
      switch( opt ) {
      case 'p':
	g_args.prefix = optarg;
	break;
      case 'q':
	g_args.min_qual = atoi(optarg);
	break;
      case 'h':
      case '?':
	print_consensus_usage();
	return 0;
	break;
      }
      opt = getopt( argc, argv, consensus_opt_str);
    }
    if(g_args.prefix.empty()){
      print_consensus_usage();
      return -1;
    }
    std::cout <<"Min Quality" << g_args.min_qual << std::endl;
    g_args.min_qual = (g_args.min_qual == 255) ? 20 : g_args.min_qual;
    res = call_consensus_from_plup(std::cin, g_args.prefix, g_args.min_qual);
  } else if (cmd.compare("removereads") == 0){
    opt = getopt( argc, argv, removereads_opt_str);
    while( opt != -1 ) {
      switch( opt ) {
      case 'i':
	g_args.bam = optarg;
	break;
      case 'p':
	g_args.prefix = optarg;
	break;
      case 'h':
      case '?':
	print_removereads_usage();
	return 0;
	break;
      }
      opt = getopt( argc, argv, removereads_opt_str);
    }
    if(argc == 0){
      print_removereads_usage();
      return -1;
    }
    uint16_t amp[100];
    for(int i = 0; i<argc;i++){
      amp[i] = atoi(argv[i]);
      std::cout << amp[i];
    }
    if(g_args.bam.empty() || g_args.prefix.empty()){
      print_removereads_usage();
      return -1;
    }
    res = rmv_reads_from_amplicon(g_args.bam, g_args.region, g_args.prefix, amp, argc);
  } else if(cmd.compare("help")){
    print_usage();
  }
  return res;
}