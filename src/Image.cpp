#include "Image.h"

Image::Image(std::vector<std::vector<Image::Pixel>> table)
    : m_table(std::move(table))
{
}

Image::Pixel::Pixel(int red, int green, int blue)
    : m_red(red)
    , m_green(green)
    , m_blue(blue)
{
}

Image::Pixel Image::GetPixel(size_t columnId, size_t rowId) const
{
    size_t negateOne = -1;
    if (columnId == negateOne) {
        columnId = m_table.size() - 1;
    }
    else if (columnId == m_table.size()) {
        columnId = 0;
    }

    if (rowId == negateOne) {
        rowId = m_table[0].size() - 1;
    }
    else if (rowId == m_table[0].size()) {
        rowId = 0;
    }

    return m_table[columnId][rowId];
}