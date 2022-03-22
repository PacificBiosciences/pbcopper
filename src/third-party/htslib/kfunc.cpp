/* The MIT License
   Copyright (C) 2010, 2013-2014, 2020 Genome Research Ltd.
   Copyright (C) 2011 Attractive Chaos <attractor@live.co.uk>
   Permission is hereby granted, free of charge, to any person obtaining
   a copy of this software and associated documentation files (the
   "Software"), to deal in the Software without restriction, including
   without limitation the rights to use, copy, modify, merge, publish,
   distribute, sublicense, and/or sell copies of the Software, and to
   permit persons to whom the Software is furnished to do so, subject to
   the following conditions:
   The above copyright notice and this permission notice shall be
   included in all copies or substantial portions of the Software.
   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
   NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
   BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
   ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
   CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.
*/

#include <pbcopper/third-party/htslib/kfunc.h>

#include <boost/math/special_functions/gamma.hpp>

#include <math.h>
#include <stdlib.h>
#include <stdint.h>

// log\binom{n}{k}
static double lbinom(int n, int k)
{
    if (k == 0 || n == k) return 0;
    namespace bm = boost::math;
    return bm::lgamma(n+1) - bm::lgamma(k+1) - bm::lgamma(n-k+1);
}

// n11  n12  | n1_
// n21  n22  | n2_
//-----------+----
// n_1  n_2  | n

// hypergeometric distribution
static double hypergeo(int n11, int n1_, int n_1, int n)
{
    return exp(lbinom(n1_, n11) + lbinom(n-n1_, n_1-n11) - lbinom(n, n_1));
}

typedef struct {
    int n11, n1_, n_1, n;
    double p;
} hgacc_t;

// incremental version of hypergenometric distribution
static double hypergeo_acc(int n11, int n1_, int n_1, int n, hgacc_t *aux)
{
    if (n1_ || n_1 || n) {
        aux->n11 = n11; aux->n1_ = n1_; aux->n_1 = n_1; aux->n = n;
    } else { // then only n11 changed; the rest fixed
        if (n11%11 && n11 + aux->n - aux->n1_ - aux->n_1) {
            if (n11 == aux->n11 + 1) { // incremental
                aux->p *= (double)(aux->n1_ - aux->n11) / n11
                    * (aux->n_1 - aux->n11) / (n11 + aux->n - aux->n1_ - aux->n_1);
                aux->n11 = n11;
                return aux->p;
            }
            if (n11 == aux->n11 - 1) { // incremental
                aux->p *= (double)aux->n11 / (aux->n1_ - n11)
                    * (aux->n11 + aux->n - aux->n1_ - aux->n_1) / (aux->n_1 - n11);
                aux->n11 = n11;
                return aux->p;
            }
        }
        aux->n11 = n11;
    }
    aux->p = hypergeo(aux->n11, aux->n1_, aux->n_1, aux->n);
    return aux->p;
}

double kt_fisher_exact(int n11, int n12, int n21, int n22, double *_left, double *_right, double *two)
{
    int i, j, max, min;
    double p, q, left, right;
    hgacc_t aux;
    int n1_, n_1, n;

    n1_ = n11 + n12; n_1 = n11 + n21; n = n11 + n12 + n21 + n22; // calculate n1_, n_1 and n
    max = (n_1 < n1_) ? n_1 : n1_; // max n11, for right tail
    min = n1_ + n_1 - n;    // not sure why n11-n22 is used instead of min(n_1,n1_)
    if (min < 0) min = 0; // min n11, for left tail
    *two = *_left = *_right = 1.;
    if (min == max) return 1.; // no need to do test
    q = hypergeo_acc(n11, n1_, n_1, n, &aux); // the probability of the current table

    if (q == 0.0) {
        /*
          If here, the calculated probablility is so small it can't be stored
          in a double, which is possible when the table contains fairly large
          numbers.  If this happens, most of the calculation can be skipped
          as 'left', 'right' and '*two' will be (to a good approximation) 0.0.
          The returned values '*_left' and '*_right' depend on which side
          of the hypergeometric PDF 'n11' sits.  This can be found by
          comparing with the mode of the distribution, the formula for which
          can be found at:
          https://en.wikipedia.org/wiki/Hypergeometric_distribution
          Note that in the comparison we multiply through by the denominator
          of the mode (n + 2) to avoid a division.
        */
        if ((int64_t) n11 * ((int64_t) n + 2) < ((int64_t) n_1 + 1) * ((int64_t) n1_ + 1)) {
            // Peak to right of n11, so probability will be lower for all
            // of the region from min to n11 and higher for at least some
            // of the region from n11 to max; hence abs(i-n11) will be 0,
            // abs(j-n11) will be > 0 and:
            *_left = 0.0; *_right = 1.0; *two = 0.0;
            return 0.0;
        } else {
            // Peak to left of n11, so probability will be lower for all
            // of the region from n11 to max and higher for at least some
            // of the region from min to n11; hence abs(i-n11) will be > 0,
            // abs(j-n11) will be 0 and:
            *_left = 1.0; *_right = 0.0; *two = 0.0;
            return 0.0;
        }
    }

    // left tail
    p = hypergeo_acc(min, 0, 0, 0, &aux);
    for (left = 0., i = min + 1; p < 0.99999999 * q && i<=max; ++i) // loop until underflow
        left += p, p = hypergeo_acc(i, 0, 0, 0, &aux);
    --i;
    if (p < 1.00000001 * q) left += p;
    else --i;
    // right tail
    p = hypergeo_acc(max, 0, 0, 0, &aux);
    for (right = 0., j = max - 1; p < 0.99999999 * q && j>=0; --j) // loop until underflow
        right += p, p = hypergeo_acc(j, 0, 0, 0, &aux);
    ++j;
    if (p < 1.00000001 * q) right += p;
    else ++j;
    // two-tail
    *two = left + right;
    if (*two > 1.) *two = 1.;
    // adjust left and right
    if (abs(i - n11) < abs(j - n11)) right = 1. - left + q;
    else left = 1.0 - right + q;
    *_left = left; *_right = right;
    return q;
}
