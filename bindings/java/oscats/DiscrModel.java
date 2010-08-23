/* OSCATS: Open-Source Computerized Adaptive Testing System
 * DiscrModel Java Wrapper Class
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

public abstract class DiscrModel extends Object
{
    protected DiscrModel(long pointer) { super(pointer); }

    public DiscrModel() {
      this(OscatsDiscrModel.new_with_params());
    }
    
    public DiscrModel(Attributes attr) {
      this(OscatsDiscrModel.new_with_params("dims", attr));
    }

    public byte getMax() { return OscatsDiscrModel.getMax(this); }
    
    public double P(byte resp, Attributes attr) {
      return OscatsDiscrModel.P(this, resp, attr);
    }
    
    public void logLikDparam(byte resp, Attributes attr, GslVector grad, GslMatrix hes) {
      OscatsDiscrModel.logLikDparam(this, resp, attr, grad, hes);
    }
    
    public String getParamName(int i) { return OscatsDiscrModel.getParamName(this, i); }
    
    public boolean hasParamName(String name) {
      return OscatsDiscrModel.hasParamName(this, name);
    }
    
    public boolean hasParam(int param) { return OscatsDiscrModel.hasParam(this, param); }
    
    public double getParam(int param) { return OscatsDiscrModel.getParam(this, param); }
    
    public double getParamByIndex(int i) { return OscatsDiscrModel.getParamByIndex(this, i); }
    
    public double getParam(String name) {
      return OscatsDiscrModel.getParamByName(this, name);
    }
    
    public double getParamByName(String name) {
      return OscatsDiscrModel.getParamByName(this, name);
    }
    
    public void setParam(int param, double x) {
      OscatsDiscrModel.setParam(this, param, x);
    }
    
    public void setParamByIndex(int i, double x) {
      OscatsDiscrModel.setParamByIndex(this, i, x);
    }
    
    public void setParam(String name, double x) {
      OscatsDiscrModel.setParamByName(this, name, x);
    }
    
    public void setParamByName(String name, double x) {
      OscatsDiscrModel.setParamByName(this, name, x);
    }

}

