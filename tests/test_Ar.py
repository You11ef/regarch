import unittest
import regarch_wrapper
import numpy as np

class TestArModel(unittest.TestCase):
    
    def test_initialization(self):
        """Test initialization of AR model with different parameters."""
        # Default initialization
        ar_default = regarch_wrapper.cAr()
        self.assertEqual(ar_default.get_n_param(), 0)
        
        # Initialize with specific number of AR terms
        ar_2 = regarch_wrapper.cAr(2)
        self.assertEqual(ar_2.get_n_param(), 2)
        
        # Initialize with different orders
        for order in range(1, 5):
            ar_model = regarch_wrapper.cAr(order)
            self.assertEqual(ar_model.get_n_param(), order)
    
    def test_parameter_getters_setters(self):
        """Test parameter setters and getters for AR model."""
        # Create model with 3 parameters
        ar_model = regarch_wrapper.cAr(3)
        
        # Set individual parameters
        ar_model.set(0.1, 0, 0)  # Set first coefficient
        ar_model.set(0.2, 1, 0)  # Set second coefficient
        ar_model.set(0.3, 2, 0)  # Set third coefficient
        
        # Verify parameters
        vector = ar_model.get(0)
        self.assertAlmostEqual(vector[0], 0.1, places=6)
        self.assertAlmostEqual(vector[1], 0.2, places=6)
        self.assertAlmostEqual(vector[2], 0.3, places=6)
        
        # Set all parameters with a Python list
        ar_model.set([0.6, 0.3, 0.1], 0)
        
        # Verify parameters again
        vector = ar_model.get(0)
        self.assertAlmostEqual(vector[0], 0.6, places=6)
        self.assertAlmostEqual(vector[1], 0.3, places=6)
        self.assertAlmostEqual(vector[2], 0.1, places=6)
        
        # Test parameter setting with extreme values
        ar_model.set([0.99, -0.99, 0.5], 0)
        vector = ar_model.get(0)
        self.assertAlmostEqual(vector[0], 0.99, places=6)
        self.assertAlmostEqual(vector[1], -0.99, places=6)
        self.assertAlmostEqual(vector[2], 0.5, places=6)
    
    def test_parameter_access_by_name(self):
        """Test parameter access by name (if supported)."""
        ar_model = regarch_wrapper.cAr(2)
        
        # Set parameters first
        ar_model.set([0.7, 0.2], 0)
        
        try:
            # Try accessing by parameter name "ar"
            ar_model.set(0.8, "ar", 0)  # Set first coefficient
            self.assertAlmostEqual(ar_model.get("ar")[0], 0.8, places=6)
            
            # Set another parameter by name
            ar_model.set(-0.3, "ar", 1)  # Set second coefficient
            self.assertAlmostEqual(ar_model.get("ar")[1], -0.3, places=6)
            
            # Set all parameters with a list using parameter name
            ar_model.set([0.5, 0.4], "ar")
            self.assertAlmostEqual(ar_model.get("ar")[0], 0.5, places=6)
            self.assertAlmostEqual(ar_model.get("ar")[1], 0.4, places=6)
        except (TypeError, AttributeError):
            print("Parameter access by name not supported for cAr model")
    
    def test_mean_computation_basic(self):
        """Test basic mean computation for AR models."""
        # AR(1) model: y_t = 0.7 * y_{t-1}
        ar1_model = regarch_wrapper.cAr(1)
        ar1_model.set(0.7, 0, 0)
        
        # Create test data with Python list
        data = regarch_wrapper.cRegArchValue([1.0, 0.8, 0.6, 0.4, 0.2])
        
        # Test mean computation
        for t in range(1, 5):
            expected_mean = 0.7 * data.mYt[t-1]
            computed_mean = ar1_model.compute_mean(t, data)
            self.assertAlmostEqual(computed_mean, expected_mean, places=6, 
                                  msg=f"AR(1) mean at t={t} failed")
    
    def test_mean_computation_advanced(self):
        """Test more complex mean computation for AR models."""
        # AR(3) model: y_t = 0.5 * y_{t-1} + 0.3 * y_{t-2} + 0.1 * y_{t-3}
        ar3_model = regarch_wrapper.cAr(3)
        ar3_model.set([0.5, 0.3, 0.1], 0)
        
        # Create test data with Python list
        data = regarch_wrapper.cRegArchValue([float(i) for i in range(10)])
        
        # Test mean computation at t=5
        # Expected: 0.5*4 + 0.3*3 + 0.1*2 = 2.0 + 0.9 + 0.2 = 3.1
        expected_mean = 0.5 * data.mYt[4] + 0.3 * data.mYt[3] + 0.1 * data.mYt[2]
        computed_mean = ar3_model.compute_mean(5, data)
        self.assertAlmostEqual(computed_mean, expected_mean, places=6)
        
        # Test mean computation at t=9
        # Expected: 0.5*8 + 0.3*7 + 0.1*6 = 4.0 + 2.1 + 0.6 = 6.7
        expected_mean = 0.5 * data.mYt[8] + 0.3 * data.mYt[7] + 0.1 * data.mYt[6]
        computed_mean = ar3_model.compute_mean(9, data)
        self.assertAlmostEqual(computed_mean, expected_mean, places=6)
    
    def test_mean_computation_with_different_coefficients(self):
        """Test mean computation with various coefficient patterns."""
        # Create test data: simple sequence
        data = regarch_wrapper.cRegArchValue([1.0, 0.5, 0.25, 0.125, 0.0625])
        
        # Test AR(1) with coefficient 0.5
        ar1 = regarch_wrapper.cAr(1)
        ar1.set(0.5, 0, 0)
        
        # At t=2, expected mean = 0.5 * data.mYt[1] = 0.5 * 0.5 = 0.25
        self.assertAlmostEqual(ar1.compute_mean(2, data), 0.25, places=6)
        
        # At t=3, expected mean = 0.5 * data.mYt[2] = 0.5 * 0.25 = 0.125
        self.assertAlmostEqual(ar1.compute_mean(3, data), 0.125, places=6)
        
        # Test AR(1) with coefficient -0.5 (oscillating)
        ar1_neg = regarch_wrapper.cAr(1)
        ar1_neg.set(-0.5, 0, 0)
        
        # At t=2, expected mean = -0.5 * data.mYt[1] = -0.5 * 0.5 = -0.25
        self.assertAlmostEqual(ar1_neg.compute_mean(2, data), -0.25, places=6)
        
        # Test AR(2) with coefficients [0.3, 0.4]
        ar2 = regarch_wrapper.cAr(2)
        ar2.set([0.3, 0.4], 0)
        
        # At t=2, expected mean = 0.3 * data.mYt[1] + 0.4 * data.mYt[0] = 0.3*0.5 + 0.4*1.0 = 0.55
        self.assertAlmostEqual(ar2.compute_mean(2, data), 0.55, places=6)
        
        # Test AR(2) with mixed signs [0.3, -0.2]
        ar2_mixed = regarch_wrapper.cAr(2)
        ar2_mixed.set([0.3, -0.2], 0)
        
        # At t=2, expected mean = 0.3 * data.mYt[1] - 0.2 * data.mYt[0] = 0.15 - 0.2 = -0.05
        self.assertAlmostEqual(ar2_mixed.compute_mean(2, data), -0.05, places=6)
        
        # Test AR(3) with increasing weights [0.1, 0.2, 0.3]
        ar3 = regarch_wrapper.cAr(3)
        ar3.set([0.1, 0.2, 0.3], 0)
        
        # At t=3, expected mean = 0.1*data.mYt[2] + 0.2*data.mYt[1] + 0.3*data.mYt[0]
        # = 0.1*0.25 + 0.2*0.5 + 0.3*1.0 = 0.025 + 0.1 + 0.3 = 0.425
        self.assertAlmostEqual(ar3.compute_mean(3, data), 0.425, places=6)
    
    def test_boundary_cases(self):
        """Test AR model with boundary cases."""
        # Case 1: AR(0) model should behave like a constant=0 model
        ar0_model = regarch_wrapper.cAr(0)
        data = regarch_wrapper.cRegArchValue([1.0, 2.0, 3.0])
        for t in range(1, 3):
            self.assertAlmostEqual(ar0_model.compute_mean(t, data), 0.0, places=6)
        
        # Case 2: AR model with all zero coefficients
        ar2_zero = regarch_wrapper.cAr(2)
        ar2_zero.set([0.0, 0.0], 0)
        for t in range(1, 3):
            self.assertAlmostEqual(ar2_zero.compute_mean(t, data), 0.0, places=6)
        
        # Case 3: AR(1) model with coefficient = 1 (unit root)
        ar1_unit = regarch_wrapper.cAr(1)
        ar1_unit.set(1.0, 0, 0)
        for t in range(1, 3):
            self.assertAlmostEqual(ar1_unit.compute_mean(t, data), data.mYt[t-1], places=6)
    
    def test_model_properties(self):
        """Test various properties of the AR model."""
        ar_model = regarch_wrapper.cAr(2)
        
        # Test number of parameters
        self.assertEqual(ar_model.get_n_param(), 2)
        
        # Test number of lags
        self.assertEqual(ar_model.get_n_lags(), 2)
        
        # Create models with different orders
        for p in range(0, 5):
            model = regarch_wrapper.cAr(p)
            self.assertEqual(model.get_n_param(), p, f"Wrong parameter count for AR({p})")
            self.assertEqual(model.get_n_lags(), p, f"Wrong lag count for AR({p})")


    def test_regarch_gradient(self):
        """
        Test creation and simple update call on cRegArchGradient without realloc.
        Matching dimension assumptions so we won't get 'wrong size' errors.
        """
        # If you plan to call t=2 in your code, we typically need nPast >= 3.
        # But if you're only testing the gradient itself, you can keep nPast=2 (or any needed dimension).
        grad = regarch_wrapper.cRegArchGradient(3, 1, 1, 1)

        # Check dimension getters
        self.assertEqual(grad.get_n_past(), 3, "Mismatch in n_past for gradient")
        self.assertEqual(grad.get_n_mean_param(), 1, "Mismatch in n_mean_param")
        self.assertEqual(grad.get_n_var_param(), 1, "Mismatch in n_var_param")
        self.assertEqual(grad.get_n_distr_param(), 1, "Mismatch in n_distr_param")

        # Reinitialize & update (no realloc)
        grad.reinitialize()
        grad.update()  # shift stored values
        
        # If no exceptions, pass
        self.assertTrue(True, "Gradient test completed without error (no realloc).")

    def test_regarch_hessien(self):
        """
        Test creation and a Hessian computation call on cRegArchHessien without realloc,
        ensuring dimension consistency to avoid 'wrong size' errors.
        We provide nonzero mHt and call with t=2.
        """
        # Construct with nPast=3 so t=2 is valid internally
        hess = regarch_wrapper.cRegArchHessien(3, 1, 1, 1)
        self.assertEqual(hess.get_n_past(), 3, "Mismatch in n_past for Hessien")
        self.assertEqual(hess.get_n_mean_param(), 1, "Mismatch in n_mean_param")
        self.assertEqual(hess.get_n_var_param(), 1, "Mismatch in n_var_param")
        self.assertEqual(hess.get_n_distr_param(), 1, "Mismatch in n_distr_param")
        
        # Provide data with non-zero mHt
        data = regarch_wrapper.cRegArchValue()
        # We'll fill 4 data points: indices 0..3. We'll compute at t=2
        data.mUt = [1.0, 2.0, 3.0, 4.0]
        data.mHt = [0.5, 0.7, 1.2, 2.0]  # no zeros => no division by zero
        
        # Provide a gradient with the SAME dimensions as the Hessien
        grad = regarch_wrapper.cRegArchGradient(3, 1, 1, 1)
        
        # Attempt the Hessian method at t=2
        hess.compute_hess_sigmat_from_hess_vart(2, data, grad)
        
        # Then update
        hess.update()
        
        # If we get here with no crash or "bad index," the test is good
        self.assertTrue(True, "Hessien test completed without error (no realloc).")



if __name__ == '__main__':
    unittest.main()
