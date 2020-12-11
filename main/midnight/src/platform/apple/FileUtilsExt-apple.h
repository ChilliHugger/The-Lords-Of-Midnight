#ifndef __FILEUTILS_EXT_APPLE_H__
#define __FILEUTILS_EXT_APPLE_H__

#include <memory>
#include <string>
#include <vector>

class FileUtilsExtApple
{
public:
    FileUtilsExtApple();
    virtual ~FileUtilsExtApple();

    std::string static getApplicationSupportPath();

};

#endif    // __FILEUTILS_EXT_APPLE_H__
