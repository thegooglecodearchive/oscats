/* OSCATS: Open-Source Computerized Adaptive Testing System
 * $Id$
 * Algorithm Java Override Bindings
 * Copyright 2010 Michael Culbertson <culbert1@illinois.edu>
 *
 *  OSCATS is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  OSCATS is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with OSCATS.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <jni.h>
#include <oscats.h>
#include "bindings_java.h"
#include "oscats_OscatsAlgorithmOverride.h"

JNIEXPORT void JNICALL
Java_oscats_OscatsAlgorithmOverride_reg
(
  JNIEnv *env,
  jclass cls,
  jlong _alg_data,
  jlong _test
)
{
  OscatsAlgorithm *alg_data = OSCATS_ALGORITHM((gpointer)_alg_data);
  OscatsTest *test = OSCATS_TEST((gpointer)_test);
  OscatsAlgorithmClass *klass = OSCATS_ALGORITHM_GET_CLASS(alg_data);
  klass->reg(alg_data, test);
}
