/* OSCATS: Open-Source Computerized Adaptive Testing System
 * $Id$
 * Examinee Java Wrapper Class
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

package oscats;

import oscats.bindings.BlacklistedMethodError;
import oscats.bindings.FIXME;
import oscats.glib.Object;

public final class Examinee extends Object
{
    protected Examinee(long pointer) { super(pointer); }

    public Examinee() {
      this(OscatsExaminee.new_with_params());
    }
    
    public Examinee(String id) {
      this(OscatsExaminee.new_with_params("id", id));
    }
    
    public void setTrueTheta(GslVector t) { 
      OscatsExaminee.setTrueTheta(this, t);
    }
    
    public GslVector getTrueTheta() {
      return OscatsExaminee.getTrueTheta(this);
    }
    
    public void setThetaHat(GslVector t) {
      OscatsExaminee.setThetaHat(this, t);
    }
    
    public GslVector getThetaHat() {
      return OscatsExaminee.getThetaHat(this);
    }
    
    public void initThetaErr(int dim) {
      OscatsExaminee.initThetaErr(this, dim);
    }

    public GslMatrix getThetaErr() {
      return OscatsExaminee.getThetaErr(this);
    }
    
    public void setTrueAlpha(Attributes attr) {
      OscatsExaminee.setTrueAlpha(this, attr);
    }
    
    public Attributes getTrueAlpha() {
      return OscatsExaminee.getTrueAlpha(this);
    }
    
    public void setAlphaHat(Attributes attr) {
      OscatsExaminee.setAlphaHat(this, attr);
    }
    
    public Attributes getAlphaHat() {
      return OscatsExaminee.getAlphaHat(this);
    }

    public int numItems() { return OscatsExaminee.numItems(this); }

}

