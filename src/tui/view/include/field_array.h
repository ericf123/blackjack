#pragma once
#include <array>
#include <cstddef>
#include <form.h>

template <size_t N_FIELDS> class FieldArray {
public:
  FieldArray() {
    // initialize all fields to null so unused fields are not handled by ncurses
    for (auto i = 0U; i < fields.size(); ++i) {
      fields[i] = nullptr;
    }
  }
  // cannot copy field array
  FieldArray(const FieldArray& other) = delete;
  FieldArray& operator=(const FieldArray& other) = delete;

  FieldArray(FieldArray&& other) {
    for (auto i = 0U; i < other.fields.size(); ++i) {
      fields[i] = other.fields[i];
      other.fields[i] = nullptr;
    }
  }

  ~FieldArray() {
    for (auto* field : fields) {
      if (field != nullptr) {
        free_field(field);
      }
    }
  }

  FIELD*& operator[](int i) { return fields[i]; }

private:
  // extra slot for null pointer at the end
  std::array<FIELD*, N_FIELDS + 1> fields;
};