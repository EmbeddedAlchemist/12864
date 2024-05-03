#pragma once
#include "DisplayHandler.hpp"
#include <array>
#include <cctype>

#include <freertos/semphr.h>

template <std::size_t _rowNum, std::size_t _colNum, const std::uint8_t *_font = u8g2_font_4x6_mf>
class _Console : public DisplayHandler, public Print {
  private:
    using LineIter = typename std::array<std::array<char, _colNum>, _rowNum>::iterator;
    using CharIter = typename std::array<char, _colNum>::iterator;

    std::array<std::array<char, _colNum>, _rowNum> buffer;
    LineIter _cLine;
    CharIter _cChar;

    LineIter _getNextLineIter();
    void _newLine();

  protected:
  public:
    virtual void onDraw(U8G2 &display);
    virtual size_t write(uint8_t);
    void clear();

    _Console();
};

extern _Console<11, 32> Console;

template <std::size_t _rowNum, std::size_t _colNum, const std::uint8_t *_font>
inline typename _Console<_rowNum, _colNum, _font>::LineIter _Console<_rowNum, _colNum, _font>::_getNextLineIter() {
    return _cLine + 1 >= buffer.end() ? buffer.begin() : _cLine + 1;
}

template <std::size_t _rowNum, std::size_t _colNum, const std::uint8_t *_font>
inline void _Console<_rowNum, _colNum, _font>::_newLine() {
    _cLine = _getNextLineIter();
    _cChar = _cLine->begin();
    std::fill(_cLine->begin(), _cLine->end(), '\0');
}

template <std::size_t _rowNum, std::size_t _colNum, const std::uint8_t *_font>
inline void _Console<_rowNum, _colNum, _font>::onDraw(U8G2 &disp) {
    disp.setFont(_font);

    int dy = 0;

    for (LineIter iter = _cLine; iter != buffer.end(); iter++) {
        int dx = 0;
        for (char ch : *iter) {
            disp.setCursor(dx, dy);
            disp.write(ch);
            dx += disp.getMaxCharWidth();
        }
        dy += disp.getMaxCharHeight();
    }

    for (LineIter iter = buffer.begin(); iter != _cLine; iter++) {
        int dx = 0;
        for (char ch : *iter) {
            disp.setCursor(dx, dy);
            disp.write(ch);
            dx += disp.getMaxCharWidth();
        }
        dy += disp.getMaxCharHeight();
    }

    return;
}

template <std::size_t _rowNum, std::size_t _colNum, const std::uint8_t *_font>
inline size_t _Console<_rowNum, _colNum, _font>::write(uint8_t ch) {
    Serial.write(ch);
    if (ch == '\n')
        _newLine();
    if (!isprint(ch))
        return 1;
    *_cChar = ch;
    _cChar++;
    if (_cChar >= _cLine->end())
        _newLine();
    return 1;
}

template <std::size_t _rowNum, std::size_t _colNum, const std::uint8_t *_font>
inline void _Console<_rowNum, _colNum, _font>::clear() {
    for (auto &row : buffer)
        std::fill(row.begin(), row.end(), '\0');
    _cLine = buffer.begin();
    _cChar = buffer[0].begin();
}

template <std::size_t _rowNum, std::size_t _colNum, const std::uint8_t *_font>
inline _Console<_rowNum, _colNum, _font>::_Console()
    : _cLine(buffer.begin()), _cChar(buffer[0].begin()) {
    clear();
}
