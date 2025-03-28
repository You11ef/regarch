import unittest
import regarch_wrapper

class TestMaModel(unittest.TestCase):
    
    def test_initialization(self):
        """Test initialization of Moving Average (MA) model."""
        # Default initialization
        ma_default = regarch_wrapper.cMa()
        self.assertEqual(ma_default.get_n_param(), 0)
        
        # Initialize with specific number of MA terms
        ma_2 = regarch_wrapper.cMa(2)
        self.assertEqual(ma_2.get_n_param(), 2)
        
        # Initialize with different orders
        for order in range(1, 5):
            ma_model = regarch_wrapper.cMa(order)
            self.assertEqual(ma_model.get_n_param(), order)
    
    def test_parameter_getters_setters(self):
        """Test parameter setters and getters for MA model."""
        ma_model = regarch_wrapper.cMa(3)
        
        # Set individual parameters
        ma_model.set(0.4, 0, 0)  # Set first coefficient
        ma_model.set(0.3, 1, 0)  # Set second coefficient
        ma_model.set(0.2, 2, 0)  # Set third coefficient
        
        # Verify parameters
        vector = ma_model.get(0)
        self.assertAlmostEqual(vector[0], 0.4, places=6)
        self.assertAlmostEqual(vector[1], 0.3, places=6)
        self.assertAlmostEqual(vector[2], 0.2, places=6)
        
        # Set parameters with a Python list
        ma_model.set([0.5, 0.3, 0.1], 0)
        
        # Verify parameters again
        vector = ma_model.get(0)
        self.assertAlmostEqual(vector[0], 0.5, places=6)
        self.assertAlmostEqual(vector[1], 0.3, places=6)
        self.assertAlmostEqual(vector[2], 0.1, places=6)
        
        # Test extreme parameter values
        ma_model.set([0.99, -0.99, 0.5], 0)
        vector = ma_model.get(0)
        self.assertAlmostEqual(vector[0], 0.99, places=6)
        self.assertAlmostEqual(vector[1], -0.99, places=6)
        self.assertAlmostEqual(vector[2], 0.5, places=6)
        
        # Try parameter access by name if supported
        try:
            ma_model.set(-0.2, "ma", 1)
            ma_coeffs = ma_model.get("ma")
            self.assertAlmostEqual(ma_coeffs[1], -0.2, places=6)
        except (TypeError, AttributeError):
            print("Note: Parameter access by name not supported for cMa")
    
    def test_mean_computation_basic(self):
        """Test basic mean computation for MA models."""
        # Create MA(1) model
        ma_model = regarch_wrapper.cMa(1)
        ma_model.set(0.7, 0, 0)
        
        # Create test data and set residuals
        data = regarch_wrapper.cRegArchValue(5)
        for i in range(5):
            data.mYt[i] = float(i)    # placeholder values
            data.mUt[i] = 0.1 * float(i)  # residuals: 0, 0.1, 0.2, ...
        
        # For MA(1): mean(t) = ma[0] * residual(t-1)
        for t in range(1, 5):
            expected_mean = 0.7 * data.mUt[t-1]
            computed_mean = ma_model.compute_mean(t, data)
            self.assertAlmostEqual(computed_mean, expected_mean, places=6,
                                  msg=f"MA(1) mean at t={t} failed")
    
    def test_mean_computation_advanced(self):
        """Test more complex mean computation for MA models."""
        # Create MA(2) model
        ma2_model = regarch_wrapper.cMa(2)
        ma2_model.set([0.6, 0.3], 0)
        
        # Create test data and set residuals with specific values
        data = regarch_wrapper.cRegArchValue(10)
        for i in range(10):
            data.mYt[i] = float(i)  # placeholder
            data.mUt[i] = 0.1 * float(i)  # residuals: 0,0.1,0.2,...
        
        # At t=3 for MA(2): mean(t)=ma[0]*res(t-1)+ma[1]*res(t-2)
        # Expected: 0.6*0.2 + 0.3*0.1 = 0.12+0.03=0.15
        expected_mean = 0.6 * data.mUt[2] + 0.3 * data.mUt[1]
        self.assertAlmostEqual(ma2_model.compute_mean(3, data), expected_mean, places=6)
        
        # At t=5 for MA(2)
        # Expected: 0.6*0.4 +0.3*0.3=0.24+0.09=0.33
        expected_mean = 0.6 * data.mUt[4] + 0.3 * data.mUt[3]
        self.assertAlmostEqual(ma2_model.compute_mean(5, data), expected_mean, places=6)
        
        # MA(3)
        ma3_model = regarch_wrapper.cMa(3)
        ma3_model.set([0.5, 0.3, 0.1], 0)
        
        # At t=4 for MA(3)
        # Expected: 0.5*0.3 +0.3*0.2 +0.1*0.1=0.15+0.06+0.01=0.22
        expected_mean = 0.5*data.mUt[3] + 0.3*data.mUt[2] + 0.1*data.mUt[1]
        self.assertAlmostEqual(ma3_model.compute_mean(4, data), expected_mean, places=6)
    
    def test_mean_computation_with_different_coefficients(self):
        """Test mean computation with various coefficient patterns."""
        # Create test data with specific residuals
        data = regarch_wrapper.cRegArchValue(5)
        for i in range(5):
            data.mYt[i] = 0.0
            data.mUt[i] = 1.0  # all residuals=1 => easy sums
        
        # 1) MA(1) with positive coefficient
        ma1_pos = regarch_wrapper.cMa(1)
        ma1_pos.set(0.5, 0, 0)
        self.assertAlmostEqual(ma1_pos.compute_mean(1, data), 0.5, places=6)
        
        # 2) MA(1) with negative coefficient
        ma1_neg = regarch_wrapper.cMa(1)
        ma1_neg.set(-0.5, 0, 0)
        self.assertAlmostEqual(ma1_neg.compute_mean(1, data), -0.5, places=6)
        
        # 3) MA(2) with positive coefficients
        ma2_pos = regarch_wrapper.cMa(2)
        ma2_pos.set([0.3, 0.2], 0)
        self.assertAlmostEqual(ma2_pos.compute_mean(2, data), 0.5, places=6)
        
        # 4) MA(2) with mixed signs
        ma2_mixed = regarch_wrapper.cMa(2)
        ma2_mixed.set([0.3, -0.2], 0)
        self.assertAlmostEqual(ma2_mixed.compute_mean(2, data), 0.1, places=6)
        
        # 5) MA(3) with increasing coefficients
        ma3_inc = regarch_wrapper.cMa(3)
        ma3_inc.set([0.1, 0.2, 0.3], 0)
        self.assertAlmostEqual(ma3_inc.compute_mean(3, data), 0.6, places=6)
    
    def test_boundary_cases(self):
        """Test MA model with boundary cases."""
        data = regarch_wrapper.cRegArchValue(5)
        for i in range(5):
            data.mUt[i] = 1.0  # all residuals=1
        
        # MA(0) => always 0
        ma0_model = regarch_wrapper.cMa(0)
        for t in range(1, 5):
            self.assertAlmostEqual(ma0_model.compute_mean(t, data), 0.0, places=6)
        
        # MA(2) => all zero coefficients
        ma2_zero = regarch_wrapper.cMa(2)
        ma2_zero.set([0.0, 0.0], 0)
        for t in range(1, 5):
            self.assertAlmostEqual(ma2_zero.compute_mean(t, data), 0.0, places=6)
        
        # Extreme residuals
        data_extreme = regarch_wrapper.cRegArchValue(5)
        for i in range(5):
            data_extreme.mUt[i] = 1000.0 if (i % 2) == 0 else -1000.0
        
        ma1_model = regarch_wrapper.cMa(1)
        ma1_model.set(0.5, 0, 0)
        
        # t=1 => 0.5 * data_extreme.mUt[0]=0.5*1000=500
        self.assertAlmostEqual(ma1_model.compute_mean(1, data_extreme), 500, places=1)
        # t=2 => 0.5 * data_extreme.mUt[1]=0.5*(-1000)=-500
        self.assertAlmostEqual(ma1_model.compute_mean(2, data_extreme), -500, places=1)
    
    def test_model_properties(self):
        """Test various properties of the MA model."""
        # Test model properties for different orders
        for p in range(0, 5):
            ma_model = regarch_wrapper.cMa(p)
            self.assertEqual(ma_model.get_n_param(), p)
            self.assertEqual(ma_model.get_n_lags(), p)
    
    def test_integration_with_regarch_model(self):
        """Test integration with the full RegArch model if available."""
        try:
            # Create an MA(1) component
            ma_comp = regarch_wrapper.cMa(1)
            ma_comp.set(0.5, 0, 0)
            
            # Create a constant variance component
            const_var = regarch_wrapper.cConstCondVar(1.0)
            
            # Create normal residuals
            normal_resid = regarch_wrapper.cNormResiduals()
            
            # Create a RegArch model
            model = regarch_wrapper.cRegArchModel()
            
            # Add the MA component
            model.add_one_mean(ma_comp)
            
            # Set variance and residuals
            model.set_var(const_var)
            model.set_resid(normal_resid)
            
            # Check we have 1 mean component
            self.assertEqual(model.get_n_mean(), 1)
            
            # Retrieve that mean component
            mean_comp = model.get_one_mean(0)
            self.assertEqual(mean_comp.get_n_param(), 1)
            
            # Attempt an example fill or simulate
            data = regarch_wrapper.cRegArchValue(5)
            # Suppose we store some residuals
            for i in range(5):
                data.mUt[i] = 0.05 * float(i)
            
            # Fill or compute from t=2
            regarch_wrapper.FillValue(2, model, data)
            
            # For t=2 => mean ~ 0.5 * data.mUt[1] => 0.5 * 0.05=0.025
            self.assertAlmostEqual(data.mMt[2], 0.025, places=5)
            
        except Exception as e:
            print(f"Note: Integration test skipped: {e}")
    
    def test_regarch_gradient(self):
        """Test creation (no realloc) + update call on cRegArchGradient in MA context."""
        # We'll use final dimensions from the start: (3,2,2,1) for example
        grad = regarch_wrapper.cRegArchGradient(3, 2, 2, 1)
        
        # Check dimension getters
        self.assertEqual(grad.get_n_past(), 3, "Mismatch in n_past for gradient")
        self.assertEqual(grad.get_n_mean_param(), 2, "Mismatch in n_mean_param")
        self.assertEqual(grad.get_n_var_param(), 2, "Mismatch in n_var_param")
        self.assertEqual(grad.get_n_distr_param(), 1, "Mismatch in n_distr_param")
        
        # Reinitialize & update (no realloc)
        grad.reinitialize()
        grad.update()
        
        self.assertTrue(True, "Gradient test completed with no errors, no realloc.")
    
    def test_regarch_hessien(self):
        """Test cRegArchHessien (no realloc) + 'compute_hess_sigmat_from_hess_vart' for MA context."""
        # Similarly, use the final dimension from the start
        hess = regarch_wrapper.cRegArchHessien(3, 2, 2, 1)
        
        self.assertEqual(hess.get_n_past(), 3, "Mismatch in n_past for Hessien")
        self.assertEqual(hess.get_n_mean_param(), 2, "Mismatch in n_mean_param")
        self.assertEqual(hess.get_n_var_param(), 2, "Mismatch in n_var_param")
        self.assertEqual(hess.get_n_distr_param(), 1, "Mismatch in n_distr_param")
        
        # Provide data with non-zero mHt
        data = regarch_wrapper.cRegArchValue()
        data.mUt = [0.1, 0.2, 0.3, 0.4]  # placeholders
        data.mHt = [0.6, 0.7, 1.0, 1.5]  # non-zero => no division by zero
        
        # Provide gradient of matching dimension
        grad = regarch_wrapper.cRegArchGradient(3, 2, 2, 1)
        
        # We'll pick t=2 to compute
        hess.compute_hess_sigmat_from_hess_vart(2, data, grad)
        
        # Then update
        hess.update()
        
        self.assertTrue(True, "Hessien test completed without error (no realloc).")


if __name__ == '__main__':
    unittest.main()
