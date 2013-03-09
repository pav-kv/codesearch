#include "fileindex.h"
#include "lister.h"
#include "queue.h"

#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>

namespace NCodesearch {

////////////////////////////////////////////////////////////////
// TLister

TLister::TLister(const TListerConfig& config, TFileIndex& fileIndex, TFileQueue& fileQueue)
    : Config(config)
    , FileIndex(fileIndex)
    , FileQueue(fileQueue)
{
    config.Print(std::cerr);
    std::cerr << "---------------------------------\n";
}

void TLister::List(const string& root) const {
    List(root.c_str());
}

void TLister::List(const char* root, unsigned int depth) const {
    DIR *dir;
    struct dirent* entry;

    if (!(dir = opendir(root))) {
        // FIXME report error
        return;
    }
    if (!(entry = readdir(dir))) {
        // FIXME report error
        return;
    }

    char fullName[4096];
    do {
        int len = snprintf(fullName, sizeof(fullName) - 1, "%s/%s", root, entry->d_name);
        fullName[len] = 0;
        if (entry->d_type == DT_DIR) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;
            if (Config.IgnoreHidden && entry->d_name[0] == '.')
                continue;
            if (Config.Recursive && depth < Config.MaxDepth)
                List(fullName, depth + 1);
        } else {
            FileIndex.Insert(fullName);
            FileQueue.Enqueue(fullName);
        }
    } while (entry = readdir(dir));
    closedir(dir);
}

////////////////////////////////////////////////////////////////
// TListerConfig

TListerConfig::TListerConfig() {
    SetDefault();
}

void TListerConfig::SetDefault() {
    Recursive = true;
    IgnoreHidden = true;
    ListDirectories = false;

    MaxDepth = -1; // infinite
}

void TListerConfig::Print(ostream& output) const {
#define OUTPUT_VALUE(name, specifier) \
    snprintf(buffer, sizeof(buffer), "%*s : " specifier, 20, #name, name); \
    output << buffer << '\n';

    char buffer[64];
    OUTPUT_VALUE(Recursive, "%d")
    OUTPUT_VALUE(IgnoreHidden, "%d")
    OUTPUT_VALUE(ListDirectories, "%d")
    output << '\n';
    OUTPUT_VALUE(MaxDepth, "%u")

#undef OUTPUT_VALUE
}

} // NCodesearch

