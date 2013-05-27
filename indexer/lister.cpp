#include "lister.h"

#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>

namespace NCodesearch {

////////////////////////////////////////////////////////////////
// TLister

TLister::TLister(const TListerConfig& config)
    : Config(config)
{
}

void TLister::List(const char* root, vector<string>& docs, unsigned depth) const {
    DIR *dir;
    struct dirent* entry;

    // TODO: report errors
    if (!(dir = opendir(root)))
        return;
    if (!(entry = readdir(dir)))
        return;

    char fullName[4096];
    do {
        int len = snprintf(fullName, sizeof(fullName), "%s/%s", root, entry->d_name);
        if (entry->d_type == DT_DIR) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;
            if (Config.IgnoreHidden && entry->d_name[0] == '.')
                continue;
            if (Config.Recursive && depth < Config.MaxDepth)
                List(fullName, docs, depth + 1);
        } else if (entry->d_type == DT_REG) {
            docs.push_back(fullName);
        }
    } while (entry = readdir(dir));
    closedir(dir);
}

////////////////////////////////////////////////////////////////
// TListerConfig

void TListerConfig::SetDefault() {
    Recursive = true;
    IgnoreHidden = true;
    ListDirectories = false;

    MaxDepth = INFINITE;
}

void TListerConfig::Print(ostream& output) const {
    char buffer[64];
    OUTPUT_CONFIG_VALUE(Recursive, "%d")
    OUTPUT_CONFIG_VALUE(IgnoreHidden, "%d")
    OUTPUT_CONFIG_VALUE(ListDirectories, "%d")
    output << '\n';
    OUTPUT_CONFIG_VALUE(MaxDepth, "%u")
}

} // NCodesearch

