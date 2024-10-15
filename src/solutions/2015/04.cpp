#include "../one_solution.h"

#ifndef USE_OPENSSL
SOLVER(2015, 4, 1, false)
(const std::string &str) {
    return "Solving this puzzle requires OpenSSL";
}

SOLVER(2015, 4, 2, false)
(const std::string &str) {
    return "Solving this puzzle requires OpenSSL";
}
#else

#include <iomanip>
#include <cstring>
#include <algorithm>

std::string bytes_to_hex(unsigned char * bytes, const size_t nbytes) {
    std::ostringstream oss;
    oss << std::setfill('0') << std::hex;

    for (size_t i = 0; i < nbytes; ++i) {
        oss << std::setw(2) << static_cast<unsigned int>(bytes[i]);
    }

    return oss.str();
}

//#include <openssl/md5.h>
//std::string md5_hex_digest(const std::string &data) {
//    unsigned char digest[MD5_DIGEST_LENGTH];
//    MD5((unsigned char *)data.c_str(), data.length(), digest);
//    return bytes_to_hex(digest, MD5_DIGEST_LENGTH);
//}

#include <openssl/evp.h>
std::string md5_hex_digest_evp(const std::string &data) {
    EVP_MD_CTX* md_ctx = EVP_MD_CTX_new();
    if (md_ctx == nullptr) {
        // TODO Handle this
        return "";
    }

    if (EVP_DigestInit_ex(md_ctx, EVP_md5(), nullptr) != 1) {
        // TODO Handle this
        EVP_MD_CTX_free(md_ctx);
        return "";
    }

    if (EVP_DigestUpdate(md_ctx, data.c_str(), data.size()) != 1) {
        // TODO Handle this
        EVP_MD_CTX_free(md_ctx);
        return "";
    }

    unsigned char md_value[EVP_MAX_MD_SIZE];
    uint md_len;
    if (EVP_DigestFinal(md_ctx, md_value, &md_len) != 1) {
        // TODO Handle this
        EVP_MD_CTX_free(md_ctx);
        return "";
    }

    EVP_MD_CTX_free(md_ctx);
    return bytes_to_hex(md_value, md_len);
}

SOLVER(2015, 4, 1, true)
(const std::string &in) {
    std::string str = in;

    int acc = 0;
    const char five_zeroes[] = {'0', '0', '0', '0', '0', '\0'};

    // Remove newlines
    str.erase(std::remove(str.begin(), str.end(), '\n'), str.end());

    while (true) {
        std::string to_hash = fmt::format("{}{}", str, acc);
        std::string hex = md5_hex_digest_evp(to_hash);

        if (strncmp(hex.c_str(), five_zeroes, 5) == 0) {
            return fmt::format("{}", acc);
        }

        if ((acc - 1) == std::numeric_limits<int>().max()) {
            fmt::println("Reached int limit! {}", acc);
            std::exit(1);
        }

        acc++;
    }
}

SOLVER(2015, 4, 2, true)
(const std::string &in) {
    std::string str = in;

    int acc = 0;
    const char six_zeroes[] = {'0', '0', '0', '0', '0', '0', '\0'};

    // Remove newlines
    str.erase(std::remove(str.begin(), str.end(), '\n'), str.end());

    while (true) {
        std::string to_hash = fmt::format("{}{}", str, acc);
        std::string hex = md5_hex_digest_evp(to_hash);

        if (strncmp(hex.c_str(), six_zeroes, 6) == 0) {
            return fmt::format("{}", acc);
        }

        if ((acc - 1) == std::numeric_limits<int>().max()) {
            fmt::println("Reached int limit! {}", acc);
            std::exit(1);
        }

        acc++;
    }
}
#endif
