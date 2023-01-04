#include <iostream>
#include <string>
#include <vector>
#include "htslib/bgzf.h"
#include "htslib/hfile.h"
#include "htslib/hts.h"
#include "htslib/sam.h"
#include "htslib/cram.h"

bool endsWith(const std::string &mainStr, const std::string &toMatch) {
    if (mainStr.size() >= toMatch.size() &&
        mainStr.compare(mainStr.size() - toMatch.size(), toMatch.size(), toMatch) == 0)
        return true;
    else
        return false;
}

int main(int argc, char** argv) {
    if (argc != 7) {
        std::cout << "RemoveCramTags DThreads CThreads REF INPUT OUTPUT TAGS\n";
        std::cout << "    DThreads - Decompression threads\n";
        std::cout << "    CThreads - Compression threads\n";
        std::cout << "    REF      - Reference fasta file\n";
        std::cout << "    INPUT    - Input SAM/BAM/CRAM\n";
        std::cout << "    OUTPUT   - Output SAM/BAM/CRAM\n";
        std::cout << "    TAGS     - List of tags, comma separated\n";
        return 0;
    }
    int threads_decomp = std::stoi(argv[1]);
    int threads_comp = std::stoi(argv[2]);
    const char* ref = argv[3];
    const char* inputf = argv[4];
    const char* outf = argv[5];

    int res;

    samFile* fi = sam_open(inputf, "r");
    res = hts_set_fai_filename(fi, ref);
    res = hts_set_threads(fi, threads_decomp);

    sam_hdr_t* samHdr = sam_hdr_read(fi);
    if (!samHdr) {
        std::cerr << "Failed to read input file header\n";
        std::terminate();
    }

    std::string o_str = argv[5];
    std::string mode = "w";
//    samFile* fo = nullptr;
    cram_fd* fc = nullptr;
    htsFile *h_out = nullptr;
    bool write_cram = false;

    if (endsWith(o_str, ".sam")) {
        h_out = hts_open(outf, mode.c_str());
    } else if (endsWith(o_str, ".bam")) {
        mode = "wb";
        h_out = hts_open(outf, mode.c_str());
    } else if (endsWith(o_str, ".cram")) {
        mode = "wc";
        h_out = hts_open(outf, "wc");
        fc = h_out->fp.cram;
        write_cram = true;
    } else if (o_str == "-") {
        std::cerr << "Using CRAM format for output file\n";
        mode = "wc";
        fc = cram_open(outf, "w");
        write_cram = true;
    } else {
        std::cerr << "File extension not recognised for output file\n";
        std::terminate();
    }
    std::cout << "Write mode: " << mode << std::endl;

    if (!write_cram) {
        hts_set_fai_filename(h_out, ref);
        hts_set_threads(h_out, threads_comp);
        res = sam_hdr_write(h_out, samHdr);
        if (res < 0) {
            std::cerr << "Failed to copy header\n";
            std::terminate();
        }
    } else {
        cram_set_option(fc, CRAM_OPT_REFERENCE, ref);
        cram_set_option(fc, CRAM_OPT_NTHREADS, threads_comp);
        cram_fd_set_header(fc, samHdr);
        res = sam_hdr_write(h_out, samHdr);
        if (res < 0) {
            std::cerr << "Failed to copy header\n";
            std::terminate();
        }
    }

    std::string tags = argv[6];
    std::vector<std::string > tag_str;
    std::vector<const char* > tagsv;
    size_t pos = 0;
    std::string delimiter = ",";
//    bool remove_all = false;
    if (tags.find(delimiter) != std::string::npos) {
        std::string token;
        while ((pos = tags.find(delimiter)) != std::string::npos) {
            token = tags.substr(0, pos);
            tag_str.push_back(token);
            tags.erase(0, pos + delimiter.length());
        }
        if (!token.empty()) {
            tag_str.push_back(token);
        }
//    } else if (tags == "ALL") {
//        remove_all = true;
    } else {
        tagsv.push_back(tags.data());
    }

    for (auto &tg: tag_str) {
        tagsv.push_back(tg.data());
    }

    std::cout << "Number of tags to remove: " << tagsv.size() << std::endl;
    bam1_t* src = bam_init1();
    long count = 0;
    long tags_removed = 0;
    while (sam_read1(fi, samHdr, src) >= 0) {
//        if (remove_all) {
//            while (true) {
//                uint8_t * data = bam_aux_first(src);
//            }
//        } else {
            for (auto &t : tagsv) {
                uint8_t* data = bam_aux_get(src, t);
                if (data != nullptr) {
                    bam_aux_del(src, data);
                    tags_removed += 1;
                }
//            }
        }

        res = sam_write1(h_out, samHdr, src);
        if (res < 0) {
            std::cerr << "Write failed" << std::endl;
            std::terminate();
        }
        count += 1;
    }
    hts_close(h_out);
    std::cout << "RemoveCramTags processed " << count << " alignments - " << tags_removed << " tags removed" << std::endl;
    return 0;
}
