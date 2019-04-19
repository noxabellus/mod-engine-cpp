#ifndef EXCEPTION_H
#define EXCEPTION_H

#include "cstd.hh"
#include "String.hh"



namespace mod {
  struct Exception {
    char* file = NULL;
    s32_t line = -1;
    s32_t column = -1;

    char* message = NULL;

    void handle () {
      if (file != NULL) free(file);
      if (message != NULL) free(message);
    }

    void print (FILE* stream = stderr) {
      fprintf(stream, "Exception");

      if (file != NULL || line != -1) {
        fprintf(stream, " at [");

        if (file != NULL) fprintf(stream, "%s", file);

        if (line != -1) {
          if (file != NULL) fprintf(stream, ":");

          fprintf(stream, "%" PRId32, line);

          if (column != -1) fprintf(stream, ":%" PRId32, column);
        }

        fprintf(stream, "]");
      }

      if (message != NULL) fprintf(stream, ": %s", message);

      fprintf(stream, "\n");
    }

    void print (String& string) {
      string.append("Exception");
      
      if (file != NULL || line != -1) {
        string.append(" at [");

        if (file != NULL) string.fmt_append("%s", file);

        if (line != -1) {
          if (file != NULL) string.append(":");

          string.fmt_append("%" PRId32, line);

          if (column != -1) string.fmt_append(":%" PRId32, column);
        }

        string.append("]");
      }
      
      if (message != NULL) string.fmt_append(": %s", message);

      string.append("\n");
    }

    void panic (FILE* stream = stderr) {
      fprintf(stream, "Unhandled ");
      print(stream);
      handle();
      abort();
    }
  };
}


/* Runtime exception with formatted error message */
template <typename ... A> NORETURN void m_asset_error (char const* file, s32_t line, s32_t column, char const* fmt, A ... args) {
  throw mod::Exception { .file = mod::str_clone(file), .line = line, .column = column, .message = mod::str_fmt(fmt, args...) };
}

/* Runtime exception with formatted error message */
template <typename ... A> NORETURN void m_asset_error (char const* file, s32_t line, char const* fmt, A ... args) {
  throw mod::Exception { .file = mod::str_clone(file), .line = line, .message = mod::str_fmt(fmt, args...) };
}

/* Runtime exception with formatted error message */
template <typename ... A> NORETURN void m_asset_error (char const* file, char const* fmt, A ... args) {
  throw mod::Exception { .file = mod::str_clone(file), .message = mod::str_fmt(fmt, args...) };
}

/* Runtime exception with formatted error message. Takes ownership of the file str */
template <typename ... A> NORETURN void m_asset_error_ex (char* file, s32_t line, s32_t column, char const* fmt, A ... args) {
  throw mod::Exception { .file = file, .line = line, .column = column, .message = mod::str_fmt(fmt, args...) };
}

/* Runtime exception with formatted error message. Takes ownership of the file str */
template <typename ... A> NORETURN void m_asset_error_ex (char* file, s32_t line, char const* fmt, A ... args) {
  throw mod::Exception { .file = file, .line = line, .message = mod::str_fmt(fmt, args...) };
}

/* Runtime exception with formatted error message. Takes ownership of the file str */
template <typename ... A> NORETURN void m_asset_error_ex (char* file, char const* fmt, A ... args) {
  throw mod::Exception { .file = file, .message = mod::str_fmt(fmt, args...) };
}

/* Runtime exception with formatted error message */
template <typename ... A> NORETURN void m_asset_error (s32_t line, s32_t column, char const* fmt, A ... args) {
  throw mod::Exception { .line = line, .column = column, .message = mod::str_fmt(fmt, args...) };
}

/* Runtime exception with formatted error message */
template <typename ... A> NORETURN void m_asset_error (s32_t line, char const* fmt, A ... args) {
  throw mod::Exception { .line = line, .message = mod::str_fmt(fmt, args...) };
}

/* Runtime exception with formatted error message */
template <typename ... A> NORETURN void m_asset_error (char const* fmt, A ... args) {
  throw mod::Exception { .message = mod::str_fmt(fmt, args...) };
}

/* Runtime assertion with exception using formatted error message if the boolean condition is not true */
#define m_asset_assert(COND,  ...) if (!(COND)) m_asset_error(__VA_ARGS__)

#endif