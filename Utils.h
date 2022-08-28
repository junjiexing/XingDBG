//
// Created by xing on 2022/8/22.
//

#pragma once
#include <QString>

namespace Utils
{
	inline QString hex(uint64_t num, int width = 16)
	{
		return QString("%1").arg(num, width, 16, QChar('0')).toUpper();
	}

    template<typename T>
    struct GetSp : public T
    {
        explicit GetSp(const T& rhs)
            : T(rhs)
        {
        }

        auto sp() const {return T::GetSP();}
    };
}
