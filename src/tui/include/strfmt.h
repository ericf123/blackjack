#pragma once
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wformat-security"

#include <memory>
#include <optional>
#include <string>

/* stolen from:
 * https://stackoverflow.com/questions/2342162/stdstring-formatting-like-sprintf
 */
template <typename... Args>
std::optional<std::string> strfmt(const std::string& format, Args... args) {
  int size_s = std::snprintf(nullptr, 0, format.c_str(), args...) +
               1; // Extra space for '\0'
  if (size_s > 0) {
    auto size = static_cast<size_t>(size_s);
    auto buf = std::make_unique<char[]>(size);
    std::snprintf(buf.get(), size, format.c_str(), args...);
    return { std::string(buf.get(), buf.get() + size -
                                        1) }; // We don't want the '\0' inside
  }

  return std::nullopt;
}

#pragma clang diagnostic pop