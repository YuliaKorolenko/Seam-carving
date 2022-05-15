#include "SeamCarver.h"

#include <algorithm>
#include <math.h>

SeamCarver::SeamCarver(Image image)
    : m_image(std::move(image))
{
}

const Image & SeamCarver::GetImage() const
{
    return m_image;
}

size_t SeamCarver::GetImageWidth() const
{
    return m_image.m_table.size();
}

size_t SeamCarver::GetImageHeight() const
{
    if (!m_image.m_table.empty()) {
        return m_image.m_table[0].size();
    }
    else {
        return 0;
    }
}

Image::Pixel SeamCarver::GetPixelShell(size_t columnId, size_t rowId) const
{
    size_t negateOne = -1;
    if (columnId == negateOne) {
        columnId = m_image.m_table.size() - 1;
    }
    else if (columnId == m_image.m_table.size()) {
        columnId = 0;
    }

    if (rowId == negateOne) {
        rowId = m_image.m_table[0].size() - 1;
    }
    else if (rowId == m_image.m_table[0].size()) {
        rowId = 0;
    }

    return m_image.m_table[columnId][rowId];
}

double SeamCarver::GetPixelEnergy(size_t columnId, size_t rowId) const
{
    const Image::Pixel leftX = GetPixelShell(columnId - 1, rowId);
    const Image::Pixel rightX = GetPixelShell(columnId + 1, rowId);
    const int RedX = rightX.m_red - leftX.m_red;
    const int GreenX = rightX.m_green - leftX.m_green;
    const int BlueX = rightX.m_blue - leftX.m_blue;
    const Image::Pixel leftY = GetPixelShell(columnId, rowId - 1);
    const Image::Pixel rightY = GetPixelShell(columnId, rowId + 1);
    const int RedY = rightY.m_red - leftY.m_red;
    const int GreenY = rightY.m_green - leftY.m_green;
    const int BlueY = rightY.m_blue - leftY.m_blue;
    const int answerX = RedX * RedX + GreenX * GreenX + BlueX * BlueX;
    const int answerY = RedY * RedY + GreenY * GreenY + BlueY * BlueY;
    return sqrt(answerX + answerY);
}

SeamCarver::Seam SeamCarver::FindSeam(const bool vertical) const
{
    const size_t width = (vertical ? GetImageHeight() : GetImageWidth()),
                 height = (vertical ? GetImageWidth() : GetImageHeight());

    std::vector<std::vector<double>> a(width, std::vector<double>(height, 0));
    for (size_t i = 0; i < height; i++) {
        a[0][i] = GetPixelEnergy(vertical ? i : 0, vertical ? 0 : i);
    }
    for (size_t j = 1; j < width; j++) {
        for (size_t i = 0; i < height; i++) {
            if (i == 0) {
                a[j][i] = GetPixelEnergy(vertical ? i : j, vertical ? j : i) + std::min(a[j - 1][i + 1], a[j - 1][i]);
            }
            else if (i == height - 1) {
                a[j][i] = GetPixelEnergy(vertical ? i : j, vertical ? j : i) + std::min(a[j - 1][i - 1], a[j - 1][i]);
            }
            else {
                a[j][i] = GetPixelEnergy(vertical ? i : j, vertical ? j : i) + std::min({a[j - 1][i - 1], a[j - 1][i], a[j - 1][i + 1]});
            }
        }
    }

    Seam horizontalSeam(width, 0);
    horizontalSeam[width - 1] = std::distance(a[width - 1].begin(), std::min_element(a[width - 1].begin(), a[width - 1].end()));
    for (int j = width - 2; j >= 0; j--) {
        size_t lastX = horizontalSeam[j + 1];
        if (lastX == 0) {
            horizontalSeam[j] = a[j][lastX] <= a[j][lastX + 1] ? lastX : lastX + 1;
        }
        else if (lastX == height - 1) {
            horizontalSeam[j] = a[j][lastX] <= a[j][lastX - 1] ? lastX : lastX - 1;
        }
        else {
            if (a[j][lastX - 1] <= a[j][lastX] && a[j][lastX - 1] <= a[j][lastX + 1]) {
                horizontalSeam[j] = lastX - 1;
            }
            else if (a[j][lastX] <= a[j][lastX - 1] && a[j][lastX] <= a[j][lastX + 1]) {
                horizontalSeam[j] = lastX;
            }
            else {
                horizontalSeam[j] = lastX + 1;
            }
        }
    }

    return horizontalSeam;
}

SeamCarver::Seam SeamCarver::FindHorizontalSeam() const
{
    return SeamCarver::FindSeam(false);
}

SeamCarver::Seam SeamCarver::FindVerticalSeam() const
{
    return SeamCarver::FindSeam(true);
}

void SeamCarver::RemoveHorizontalSeam(const Seam & seam)
{
    for (size_t j = 0; j < GetImageWidth(); j++) {
        m_image.m_table[j].erase(m_image.m_table[j].begin() + seam[j]);
    }
}

void SeamCarver::RemoveVerticalSeam(const Seam & seam)
{
    for (size_t i = 0; i < seam.size(); ++i) {
        for (size_t j = seam[i]; j < GetImageWidth() - 1; ++j) {
            m_image.m_table[j][i] = m_image.m_table[j + 1][i];
        }
    }
    m_image.m_table.pop_back();
}
