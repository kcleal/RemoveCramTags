#include <iostream>
#include <string>
#include <vector>
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

    std::string o_str = argv[5];
    std::string mode = "w";
    samFile* fo = nullptr;
    cram_fd* fc = nullptr;
    bool write_cram = false;

    if (endsWith(o_str, ".sam")) {
        fo = sam_open(outf, mode.c_str());
    } else if (endsWith(o_str, ".bam")) {
        mode = "wb";
        fo = sam_open(outf, mode.c_str());
    } else if (endsWith(o_str, ".cram")) {
        mode = "wc";
        fc = cram_open(outf, "w");
        write_cram = true;
    } else if (o_str == "-") {
        std::cerr << "Using CRAM format for output file\n";
        mode = "wc";
        fc = cram_open(outf, "w");
        write_cram = true;
    } else {
        std::cerr << "File extension not recognised for output file\n";
        return -1;
    }
    std::cout << "Write mode: " << mode << std::endl;
    if (!write_cram) {
        res = hts_set_fai_filename(fo, ref);
        res = hts_set_threads(fo, threads_comp);
    } else {
    }

    sam_hdr_t* samHdr = sam_hdr_read(fi);
    if (!samHdr) {
        std::cerr << "Failed to read input file header\n";
        return -1;
    }

    if (!write_cram) {
        res = sam_hdr_write(fo, samHdr);
        if (res < 0) {
            std::cerr << "Failed to copy header\n";
            return -1;
        }
    } else {
        cram_fd_set_header(fc, samHdr);
    }

    std::string tags = argv[6];
    std::vector<const char* > tagsv;
    size_t pos = 0;
    std::string delimiter = ",";
    if (tags.find(delimiter) != std::string::npos) {
        std::string token;
        while ((pos = tags.find(delimiter)) != std::string::npos) {
            token = tags.substr(0, pos);
            tagsv.push_back(token.c_str());
            tags.erase(0, pos + delimiter.length());
        }
        if (!token.empty()) {
            tagsv.push_back(token.c_str());
        }
    } else {
        tagsv.push_back(tags.c_str());
    }

    std::cout << "Number of tags to remove: " << tagsv.size() << std::endl;
    bam1_t* src = bam_init1();
    long count = 0;
    long tags_removed = 0;
    cram_container *c = nullptr;
    while (sam_read1(fi, samHdr, src) >= 0) {
        for (auto &t : tagsv) {
            uint8_t* data = bam_aux_get(src, t);
            if (data != nullptr) {
                bam_aux_del(src, data);
                tags_removed += 1;
            }
        }
        if (!write_cram) {
            res = sam_write1(fo, samHdr, src);
        } else {
            res = sam_write1(fc, samHdr, src);
        }

        count += 1;
    }
    std::cout << "RemoveCramTags processed " << count << " alignments - " << tags_removed << " tags removed" << std::endl;
    return 0;
}
