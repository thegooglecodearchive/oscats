/* OSCATS: Open-Source Computerized Adaptive Testing System
 * ContModel Java Wrapper Class
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

public abstract class ContModel extends Object
{
    protected ContModel(long pointer) { super(pointer); }

    public ContModel() {
      this(OscatsContModel.new_with_params());
    }
    
    public ContModel(BitArray dims) {
      this(OscatsContModel.new_with_params("dims", dims));
    }

    public byte getMax() { return OscatsContModel.getMax(this); }
    
    public double P(byte resp, GslVector theta, Covariates covariates) {
      return OscatsContModel.P(this, resp, theta, covariates);
    }
    
    public double distance(GslVector theta, Covariates covariates) {
      return OscatsContModel.distance(this, theta, covariates);
    }
    
    public void logLikDtheta(byte resp, GslVector theta, Covariates covariates, GslVector grad, GslMatrix hes) {
      OscatsContModel.logLikDtheta(this, resp, theta, covariates, grad, hes);
    }
    
    public void logLikDparam(byte resp, GslVector theta, Covariates covariates, GslVector grad, GslMatrix hes) {
      OscatsContModel.logLikDparam(this, resp, theta, covariates, grad, hes);
    }
    
    public void fisherInf(GslVector theta, Covariates covariates, GslMatrix I) {
      OscatsContModel.fisherInf(this, theta, covariates, I);
    }
    
    public String getParamName(int i) { return OscatsContModel.getParamName(this, i); }
    
    public boolean hasParamName(String name) {
      return OscatsContModel.hasParamName(this, name);
    }
    
    public boolean hasParam(int param) { return OscatsContModel.hasParam(this, param); }
    
    public double getParam(int param) { return OscatsContModel.getParam(this, param); }
    
    public double getParamByIndex(int i) { return OscatsContModel.getParamByIndex(this, i); }
    
    public double getParam(String name) {
      return OscatsContModel.getParamByName(this, name);
    }
    
    public double getParamByName(String name) {
      return OscatsContModel.getParamByName(this, name);
    }
    
    public void setParam(int param, double x) {
      OscatsContModel.setParam(this, param, x);
    }
    
    public void setParamByIndex(int i, double x) {
      OscatsContModel.setParamByIndex(this, i, x);
    }
    
    public void setParam(String name, double x) {
      OscatsContModel.setParamByName(this, name, x);
    }
    
    public void setParamByName(String name, double x) {
      OscatsContModel.setParamByName(this, name, x);
    }

    public String getCovariateName(int i) { return OscatsContModel.getCovariateName(this, i); }
    
    public boolean hasCovariate(int name) { return OscatsContModel.hasCovariate(this, name); }
    
    public boolean hasCovariate(String name) { return OscatsContModel.hasCovariateName(this, name); }
    
    public boolean hasCovariateName(String name) { return OscatsContModel.hasCovariateName(this, name); }
    
    public void setCovariateName(int i, String name) {
      OscatsContModel.setCovariateNameStr(this, i, name);
    }

}

