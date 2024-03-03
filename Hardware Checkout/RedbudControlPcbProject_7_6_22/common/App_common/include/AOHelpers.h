/**
 * @file AOHelpers.h
 *
 * @author bvoigt
 *
 * Note:  The QP Framework assigns 1 as the lowest priority AO in the system, so these
 * AOStart and AOStartMulti calls should be made in reverse order of priority.
 */

#ifndef AOHELPERS_H_
#define AOHELPERS_H_

#include "qpcpp.hpp"

namespace COMMON {
using namespace QP;

class AOPriorityAssigner
{
protected:
    static int nextPriority;
};
int AOPriorityAssigner::nextPriority = 1;

template<int QSize>
class AOStart : AOPriorityAssigner
{
    QEvt const *QSto[QSize];

public:
    AOStart(QP::QActive* ao)
    {
        if (ao != nullptr) {
            ao->start(nextPriority, QSto, QSize, nullptr, 0u);
        }

        // Always increment nextPriority even if we get nullptr.
        nextPriority++;
    }
};

template<int QSize, int count>
class AOStartMulti : AOPriorityAssigner
{
    QEvt const *QSto[count][QSize];

public:
    AOStartMulti(std::array<QP::QActive* const, count> const& aoFirst)
    {
        for( int i = 0; i < count; ++i ) {
            if (aoFirst[i] != nullptr) {
                aoFirst[i]->start(nextPriority, QSto[i], QSize, nullptr, 0u);
            }

            // Always increment nextPriority even if we get nullptr.
            nextPriority++;
        }
    }
};

} // namespace COMMON

#endif // AOHELPERS_H_
