/*
  ==============================================================================

    Buffer.cpp
    Created: 21 Oct 2021 1:47:25pm
    Author:  tanch

  ==============================================================================
*/

#include "Buffer.h"
#include <algorithm>

Buffer::Buffer() {}

Buffer::Buffer(int size)
{
    for (int i=0; i<2; i++) {
        _wbuf[i].resize(size);
        std::fill(_wbuf[i].begin(), _wbuf[i].end(), 0);
    }
    _wcur = 0;
    _rdone = false;
}

void Buffer::write(std::vector<float> &data)
{
    assert(_wbuf[_wcur].size() == data.size());
    _wbuf[_wcur] = data;
    if (_rdone) {
        _wcur = 1 - _wcur;
        _rdone = false;
    }
}

std::vector<float>& Buffer::read()
{
    _rbuf = _wbuf[1-_wcur];
    _rdone = true;
    return _rbuf;
}
