/*
 *  Copyright (C) 2015, Mike Walters <mike@flomp.net>
 *
 *  This file is part of inspectrum.
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <QCache>
#include <QWidget>
#include "fft.h"
#include "inputsource.h"
#include "plot.h"

#include <memory>
#include <math.h>

static const double Tau = M_PI * 2.0;

class TileCacheKey;

class SpectrogramPlot : public Plot
{
    Q_OBJECT

public:
    SpectrogramPlot(SampleSource<std::complex<float>> *src);

    void paintMid(QPainter &painter, QRect &rect, range_t<off_t> sampleRange);

    QSize sizeHint() const;
    int getHeight();
    int getStride();
    off_t lineToSample(off_t line);

    SampleSource<std::complex<float>> *inputSource = nullptr;

public slots:
    void setSampleRate(int rate);
    void setFFTSize(int size);
    void setPowerMax(int power);
    void setPowerMin(int power);
    void setZoomLevel(int zoom);

protected:
    void mouseReleaseEvent(QMouseEvent * event);
    void mouseMoveEvent(QMouseEvent * event);
    void mousePressEvent(QMouseEvent * event);


private:
    const int linesPerGraduation = 50;
    const int tileSize = 65536; // This must be a multiple of the maximum FFT size

    std::unique_ptr<FFT> fft;
    std::unique_ptr<float[]> window;
    fftwf_complex *lineBuffer = nullptr;
    QCache<TileCacheKey, QPixmap> pixmapCache;
    QCache<TileCacheKey, float> fftCache;
    uint colormap[256];

    int sampleRate;
    int fftSize;
    int zoomLevel;
    float powerMax;
    float powerMin;

    QPixmap* getPixmapTile(off_t tile);
    float* getFFTTile(off_t tile);
    void getLine(float *dest, off_t sample);
    void paintCursors(QPainter *painter, QRect rect);
    int sampleToLine(off_t sample);
    QString sampleToTime(off_t sample);
    int linesPerTile();
};

class TileCacheKey
{

public:
    TileCacheKey(int fftSize, int zoomLevel, off_t sample) {
        this->fftSize = fftSize;
        this->zoomLevel = zoomLevel;
        this->sample = sample;
    }

    bool operator==(const TileCacheKey &k2) const {
        return (this->fftSize == k2.fftSize) &&
               (this->zoomLevel == k2.zoomLevel) &&
               (this->sample == k2.sample);
    }

    int fftSize;
    int zoomLevel;
    off_t sample;
};
