#pragma once

#include "field_array.h"
#include "view.h"
#include <form.h>

static inline void deleteForm(FORM *form) {
  if (form != nullptr) {
    free_form(form);
  }
}

using FormPtr = std::unique_ptr<FORM, decltype(&deleteForm)>;

template <size_t NUM_FIELDS> class FormView : public View {
public:
  FormView(int height, int width, int starty, int startx)
      : View(height, width, starty, startx), form(nullptr, &deleteForm) {}

  FormView(FormView &&view) = default;
  virtual ~FormView() = default;

protected:
  // TODO: use unique_ptr
  // FORM *form;
  FieldArray<NUM_FIELDS> fields;
  FormPtr form;
};