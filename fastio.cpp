#include <cstdio> // for C-Style IO (fread, fwrite)
#include <cctype> // for isdigit, isspace
#include <type_traits> // for is_integral_v, is floating_point_v
#include <string> // std::string

// buffer size (1MB)
constexpr int BUF_SIZE = 1 << 20;

// FastIO
class FastIO {
private:
    // --- member variables for input ---
    char in_buf[BUF_SIZE]; // input buffer
    int read_idx;          // current position
    int next_idx;          // end position of data filled buffer
    bool __END_FLAG__;     // EOF Flag
    bool __GETLINE_FLAG__; // EOF check flag of ReadLine

    // --- member variables for output ---
    char out_buf[BUF_SIZE]; // output buffer
    int write_idx;          // current write position of output buffer

    // --- helper function for input ---
    // read one of character from input buffer.
    inline char _read_char() {
        // read buffer
        if (read_idx == next_idx) {
            // fill buffer from stdin with fread
            next_idx = fread(in_buf, 1, BUF_SIZE, stdin);
            // EOF
            if (next_idx == 0) {
                // EOF Flag true
                __END_FLAG__ = true;
                return 0;
            }
            // reset position to buffer start pos
            read_idx = 0;
        }
        // print current character, increase read index
        return in_buf[read_idx++];
    }

    // check is blank character
    inline bool _is_blank(char c) {
        return c == ' ' || c == '\n' || c == '\r' || c == '\t';
    }

    // --- Output 관련 helper function ---
    // write one of character to output buffer.
    inline void _write_char(char c) {
        // buffer is full
        if (write_idx == BUF_SIZE) {
            // flush
            _flush_output();
        }
        // save character to buffer and increase index
        out_buf[write_idx++] = c;
    }

    // flush
    inline void _flush_output() {
        // there is context to write to buffer
        if (write_idx > 0) {
            // real output with fwrite
            fwrite(out_buf, 1, write_idx, stdout);
            // reset write index
            write_idx = 0;
        }
    }

    // integer digit calculation
    template<typename T>
    inline int _get_size(T n) {
        int ret = 1;
        
        // digit cal without sign when minus
        for (n = n >= 0 ? n : -n; n >= 10; n /= 10) ret++;
        return ret;
    }

public:
    // init buffer index and flag with create FastIO obj
    FastIO() : read_idx(0), next_idx(0), __END_FLAG__(false), __GETLINE_FLAG__(false), write_idx(0) {}
    
    // flush context of buffer with remove FastIO obj
    ~FastIO() {
        _flush_output();
    }

    // check stream status
    // convert FIO obj to bool type
    explicit operator bool() const {
      return !__END_FLAG__;
    }

    // --- Input Method ---
    // skip blank and read valid char
    inline char ReadChar() {
        char ret = _read_char();
        // blank and not EOF
        while (_is_blank(ret) && !__END_FLAG__) {
            // read
            ret = _read_char();
        }
        return ret;
    }

    // integral type
    template<typename T, typename std::enable_if_t<std::is_integral_v<T>>* = nullptr>
    inline T ReadInt() {
        T ret = 0;
        char cur = _read_char();
        bool is_negative = false;

        // skip blank
        while (_is_blank(cur) && !__END_FLAG__) {
            cur = _read_char();
        }

        // minus sign
        if (cur == '-') {
            is_negative = true;
            cur = _read_char();
        }
        // plus sign
        else if (cur == '+') {
            cur = _read_char();
        }

        // parse with read numbers
        while (cur >= '0' && cur <= '9' && !__END_FLAG__) {
            ret = ret * 10 + (cur - '0');
            cur = _read_char();
        }
        
        // if minus, return with sign
        return is_negative ? -ret : ret;
    }

    // read word
    inline std::string ReadString() {
        std::string ret;
        char cur = _read_char();

        // skip blank
        while (_is_blank(cur) && !__END_FLAG__) {
            cur = _read_char();
        }

        // read until cur is not blank or EOF
        while (!_is_blank(cur) && !__END_FLAG__ && cur != 0) {
            // 문자를 문자열에 추가
            ret.push_back(cur);
            cur = _read_char();
        }
        
        // if last char is EOF, set flag
        if (cur == 0) __END_FLAG__ = true;
        return ret;
    }

    // convert to double with stod() after read string
    inline double ReadDouble() {
        return std::stod(ReadString());
    }

    // read Line.
    inline std::string ReadLine() {
        std::string ret;
        char cur = _read_char();
        
        // skip '\n' from before input
        while (cur == '\n' && !__END_FLAG__) {
             cur = _read_char();
        }

        // read until cur is \n or EOF
        while (cur != '\n' && !__END_FLAG__ && cur != 0) {
            // add char to string
            ret.push_back(cur);
            cur = _read_char();
        }

        // when loop finished with EOF, set GETLINE Flag
        if (cur == 0) __GETLINE_FLAG__ = true;
        return ret;
    }

    // --- Output Method ---
    // write(print) one char
    inline void WriteChar(char c) {
        _write_char(c);
    }

    // print integral
    template<typename T, typename std::enable_if_t<std::is_integral_v<T>>* = nullptr>
    inline void WriteInt(T n) {
        // cal digit
        int sz = _get_size(n);

        // flush buffer when the buffer is full.
        if (write_idx + sz + (n < 0 ? 1 : 0) >= BUF_SIZE) {
            _flush_output();
        }

        // when minus
        if (n < 0) {
            _write_char('-');
            n = -n;
        }
        
        // when zero
        if (n == 0) {
            _write_char('0');
            return;
        }

        // temp buffer
        char temp_buf[20];
        int temp_idx = 0;

        while (n > 0) {
            temp_buf[temp_idx++] = (n % 10) + '0';
            n /= 10;
        }

        while (temp_idx > 0) {
            _write_char(temp_buf[--temp_idx]);
        }
    }

    // string
    inline void WriteString(const std::string& s) {
        for (char c : s) {
            _write_char(c);
        }
    }

    // print double with converting to string
    inline void WriteDouble(double d) {
        WriteString(std::to_string(d));
    }

    // --- Operator Overload ---
    // >> operator for char type
    FastIO& operator>>(char& i) {
        i = ReadChar();
        // chaining을 위한 obj ref 반환
        return *this;
    }

    // >> for std::string
    FastIO& operator>>(std::string& i) {
        i = ReadString();
        return *this;
    }

    // >> for int/float
    template<typename T, typename std::enable_if_t<std::is_arithmetic_v<T>>* = nullptr>
    FastIO& operator>>(T& i) {
        // float
        if constexpr (std::is_floating_point_v<T>) {
            i = ReadDouble();
        }
        // integral
        else if constexpr (std::is_integral_v<T>) {
            i = ReadInt<T>();
        }
        return *this;
    }

    // << operator for print char type
    FastIO& operator<<(char i) {
        WriteChar(i);
        return *this;
    }

    // << for std::string
    FastIO& operator<<(const std::string& i) {
        WriteString(i);
        return *this;
    }

    // << for ㅊ-Style string (const char *)
    FastIO& operator<<(const char* i) {
        WriteString(std::string(i));
        return *this;
    }

    // << for int or float
    template<typename T, typename std::enable_if_t<std::is_arithmetic_v<T>>* = nullptr>
    FastIO& operator<<(T i) {
        if constexpr (std::is_floating_point_v<T>) {
            WriteDouble(i);
        } else if constexpr (std::is_integral_v<T>) {
            WriteInt<T>(i);
        }
        return *this;
    }
};
