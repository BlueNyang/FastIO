# FastIO
## Fast I/O for Cpp

## 1. libraries

```c++
#include <cstdio> // C-Style IO (fread, fwrite) 사용을 위한 header
#include <cctype> // 문자 분류 함수 (isdigit, isspace)
#include <type_traits> // type 특성 (is_integral_v, is_floating_point_v)
#include <string> // std::string
```

## 2. buffer 사이즈

```c++
// buffer 사이즈 (1MB)
constexpr int BUF_SIZE = 1 << 20;
```

## 3. 멤버 변수

```C++
private:
    // --- Input 관련 member variables ---
    char in_buf[BUF_SIZE]; // 입력 버퍼
    int read_idx;                // 현재 위치
    int next_idx;                // 버퍼에 채워진 데이터 끝 위치
    bool __END_FLAG__;           // 파일의 끝 (EOF) 플래그
    bool __GETLINE_FLAG__;       // ReadLine의 EOF 확인 플래그

    // --- Output 관련 member variables ---
    char out_buf[BUF_SIZE]; // 출력 버퍼
    int write_idx;                // 출력 버퍼의 현재 쓰기 위치
```

## 4. private 함수

```C++
private:
    // --- Input 관련 helper function ---
    // 입력 버퍼에서 한 문자 read. 버퍼가 비면 새로 채움
    inline char _read_char() {
        // 버퍼 읽음
        if (read_idx == next_idx) {
            // fread로 stdin에서 버퍼 채움
            next_idx = fread(in_buf, 1, BUF_SIZE, stdin);
            // EOF
            if (next_idx == 0) {
                // EOF Flag true
                __END_FLAG__ = true;
                return 0;
            }
            // buffer 시작지점으로 위치 초기화
            read_idx = 0;
        }
        // 현재 문자 출력, 읽기 index 증가
        return in_buf[read_idx++];
    }

    // 공백 문자인지 여부 확인
    inline bool _is_blank(char c) {
        return c == ' ' || c == '\n' || c == '\r' || c == '\t';
    }

    // --- Output 관련 helper function ---
    // 출력 버퍼에 한 문자를 write. 버퍼가 가득차면 flush
    inline void _write_char(char c) {
        // 버퍼 가득 참
        if (write_idx == BUF_SIZE) {
            // flush
            _flush_output();
        }
        // 버퍼에 문자 저장 후 쓰기 index 증가
        out_buf[write_idx++] = c;
    }

    // flush
    inline void _flush_output() {
        // 버퍼에 쓸 내용이 있음
        if (write_idx > 0) {
            // fwrite로 실제 출력
            fwrite(out_buf, 1, write_idx, stdout);
            // write index 초기화
            write_idx = 0;
        }
    }

    // 정수 n자리 수 계산
    template<typename T>
    inline int _get_size(T n) {
        int ret = 1;
        
        // 음수이면 부호 제거 하고 자릿수 계산
        // 예를 들어 100 이면 2번 증가되어 3자리 수
        for (n = n >= 0 ? n : -n; n >= 10; n /= 10) ret++;
        return ret;
    }
```

## 5. 생성자 및 소멸자

```c++
public:
    // FastIO 객체 생성 시 버퍼 인덱스와 플래그 초기화
    FastIO() : read_idx(0), next_idx(0), __END_FLAG__(false), __GETLINE_FLAG__(false), write_idx(0) {}
    
    // FastIO 객체 소멸 시 출력 버퍼 내용 flush
    ~FastIO() {
        _flush_output();
    }
```

## 6. Stream 상태 확인

```c++
    // Stream 상태 확인
    // FIO 객체를 bool 타입 변환 시 EOF가 아니면 true 반환 == 유효함
    explicit operator bool() const {
      return !__END_FLAG__;
    }
```

## 7. Input 메서드 (read)

```c++
    // 공백 skip하고 유효한 문자 read
    inline char ReadChar() {
        char ret = _read_char();
        // 공백이고 EOF가 아니면 계속 읽기
        while (_is_blank(ret) && !__END_FLAG__) {
            ret = _read_char();
        }
        return ret;
    }
```

### 7.1. 타입 별 read
```c++
    // 정수 타입
    template<typename T, typename std::enable_if_t<std::is_integral_v<T>>* = nullptr>
    inline T ReadInt() {
        T ret = 0;
        char cur = _read_char();
        bool is_negative = false;

        // 공백 skip
        while (_is_blank(cur) && !__END_FLAG__) {
            cur = _read_char();
        }

        // 음수 부호 처리
        if (cur == '-') {
            is_negative = true;
            cur = _read_char();
        }
        // 양수 부호 처리 (cur이 +이면)
        else if (cur == '+') {
            cur = _read_char();
        }

        // 숫자 읽고 파싱
        while (cur >= '0' && cur <= '9' && !__END_FLAG__) {
            ret = ret * 10 + (cur - '0');
            cur = _read_char();
        }
        
        // 마지막 문자가 EOF이면 flag는 _read_char()에서 처리
        // 음수이면 부호 적용하여 반환
        return is_negative ? -ret : ret;
    }

    // 공백이 아닌 문자들로 이루어진 단어 read
    inline std::string ReadString() {
        std::string ret;
        char cur = _read_char();

        // 공백 skip
        while (_is_blank(cur) && !__END_FLAG__) {
            cur = _read_char();
        }

        // EOF 공백이나 EOF가 아닐 때까지 read
        while (!_is_blank(cur) && !__END_FLAG__ && cur != 0) {
            // 문자를 문자열에 추가
            ret.push_back(cur);
            cur = _read_char();
        }
        
        // 마지막 문자가 EOF면 flag 설정
        if (cur == 0) __END_FLAG__ = true;
        return ret;
    }

    // 문자열을 읽고 stod()를 사용해 double로 변환
    inline double ReadDouble() {
        return std::stod(ReadString());
    }

    // 한 줄 읽기. 개행문자나 EOF까지 읽음
    inline std::string ReadLine() {
        std::string ret;
        char cur = _read_char();
        
        // 이전 입력의 남은 개행 문자 건너뜁
        while (cur == '\n' && !__END_FLAG__) {
             cur = _read_char();
        }

        // \n이나 EOF까지 read
        while (cur != '\n' && !__END_FLAG__ && cur != 0) {
            // 문자열에 문자 추가
            ret.push_back(cur);
            cur = _read_char();
        }

        // EOF로 루프 종료 시 GETLINE Flag 설정
        if (cur == 0) __GETLINE_FLAG__ = true;
        return ret;
    }
```

## 8. Output 메서드 (Write)

```c++
    // 한 문자 쓰기(출력)
    inline void WriteChar(char c) {
        _write_char(c);
    }
```
### 8.1. 각 type 별 write

```c++
    // 정수 출력
    template<typename T, typename std::enable_if_t<std::is_integral_v<T>>* = nullptr>
    inline void WriteInt(T n) {
        // 자릿수 계산
        int sz = _get_size(n);

        // 버퍼 공간 부족 시 버퍼 flush.
        // 수가 음수 이면 부호 고려하여 사이즈 확인 
        if (write_idx + sz + (n < 0 ? 1 : 0) >= BUF_SIZE) {
            _flush_output();
        }

        // 음수 처리
        if (n < 0) {
            _write_char('-');
            n = -n;
        }
        
        // 0이면 따로 처리
        if (n == 0) {
            _write_char('0');
            return;
        }

        // 숫자를 하나씩 가지는 임시 버퍼
        char temp_buf[20];
        int temp_idx = 0;

        // 숫자를 낮은 자리 부터 버퍼에 넣음
        while (n > 0) {
            // index 값은 커짐. 숫자를 문자로 변환
            temp_buf[temp_idx++] = (n % 10) + '0';
            n /= 10;
        }

        // 끝에 있는 index를 다시 줄이면서 출력(큰 자리수 부터)
        while (temp_idx > 0) {
            // 역순 출력
            _write_char(temp_buf[--temp_idx]);
        }
    }

    // 문자열 출력
    inline void WriteString(const std::string& s) {
        // for문으로 각 문자 출력
        for (char c : s) {
            _write_char(c);
        }
    }

    // double을 문자열로 변환한 후 출력
    inline void WriteDouble(double d) {
        WriteString(std::to_string(d));
    }
```
```c++
    // --- Operator Overload ---
    // char 타입을 위한 >> 연산자
    FastIO& operator>>(char& i) {
        i = ReadChar();
        // chaining을 위한 obj ref 반환
        return *this;
    }

    // std::string을 위한 >> 연산자
    FastIO& operator>>(std::string& i) {
        i = ReadString();
        return *this;
    }

    // 정수, 실수 입력을 위한 >> 연산자
    // std::enable_if_t, std::is_arithmetic_v를 사용해 산술 type에만 적용
    template<typename T, typename std::enable_if_t<std::is_arithmetic_v<T>>* = nullptr>
    FastIO& operator>>(T& i) {
        // 실수 타입
        if constexpr (std::is_floating_point_v<T>) {
            i = ReadDouble();
        }
        // 정수 타입
        else if constexpr (std::is_integral_v<T>) {
            i = ReadInt<T>();
        }
        return *this;
    }

    // char 출력을 위한 << 연산자
    FastIO& operator<<(char i) {
        WriteChar(i);
        return *this;
    }

    // std::string을 위한 << 연산자
    FastIO& operator<<(const std::string& i) {
        WriteString(i);
        return *this;
    }

    // C-Style 문자열 (const char *)을 위한 << 연산자
    FastIO& operator<<(const char* i) {
        WriteString(std::string(i));
        return *this;
    }

    // 정수 및 실수를 위한 << 연산자
    // 마찬가지로 산술 type에만 적용
    template<typename T, typename std::enable_if_t<std::is_arithmetic_v<T>>* = nullptr>
    FastIO& operator<<(T i) {
        if constexpr (std::is_floating_point_v<T>) {
            WriteDouble(i);
        } else if constexpr (std::is_integral_v<T>) {
            WriteInt<T>(i);
        }
        return *this;
    }
```
