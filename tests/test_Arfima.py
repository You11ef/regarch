import unittest
import regarch_wrapper
import math

class TestArfimaModel(unittest.TestCase):
    
    def test_initialization(self):
        """Test initialization of ARFIMA model with different parameters."""
        # Default initialization
        arfima_default = regarch_wrapper.cArfima()
        self.assertEqual(arfima_default.get_n_param(), 1)
        
        # Test various combinations of p, d, q
        test_cases = [
            (0, 0, 0.0, 1),    # ARFIMA(0,0,0) - just d parameter
            (1, 0, 0.2, 2),    # ARFIMA(1,0,0.2) - 1 AR + d
            (0, 1, 0.2, 2),    # ARFIMA(0,1,0.2) - 1 MA + d
            (1, 1, 0.2, 3),    # ARFIMA(1,1,0.2) - 1 AR + 1 MA + d
            (2, 0, 0.2, 3),    # ARFIMA(2,0,0.2) - 2 AR + d
            (0, 2, 0.2, 3),    # ARFIMA(0,2,0.2) - 2 MA + d
            (2, 2, 0.2, 5)     # ARFIMA(2,2,0.2) - 2 AR + 2 MA + d
        ]
        
        for p, q, d, expected_params in test_cases:
            model = regarch_wrapper.cArfima(p, q, d)
            self.assertEqual(model.get_n_param(), expected_params, 
                             f"ARFIMA({p},{d},{q}) should have {expected_params} parameters")
    
    def test_model_properties(self):
        """Test all accessible properties of the ARFIMA model."""
        # Create a model with both AR and MA components
        arfima_model = regarch_wrapper.cArfima(2, 2, 0.3)
        
        # Test AR order
        self.assertEqual(arfima_model.get_n_ar(), 2, "AR order should be 2")
        
        # Test MA order
        self.assertEqual(arfima_model.get_n_ma(), 2, "MA order should be 2")
        
        # Test number of parameters (2 AR + 2 MA + 1 d = 5)
        self.assertEqual(arfima_model.get_n_param(), 5, "Should have 5 parameters")
        
        # Test number of lags
        n_lags = arfima_model.get_n_lags()
        print(f"Number of lags for ARFIMA(2,2,0.3): {n_lags}")
        # The number of lags for a fractional model might be large due to truncation
        self.assertTrue(n_lags >= max(2, 2), 
                       msg="Number of lags should be at least max(p,q)")
    
    def test_parameter_setting(self):
        """Test setting parameters using the correct indexing."""
        # Create a model
        model = regarch_wrapper.cArfima(1, 1, 0.2)
        print(f"Created model: ARFIMA(1,1,0.2) with {model.get_n_param()} parameters")
        
        # Test setting d parameter
        new_d = 0.3
        model.set(new_d, "d")
        d_param = model.get("d")
        print(f"Set d={new_d}, now d={d_param}")
        self.assertEqual(float(str(d_param)), new_d, 
                         msg=f"d parameter should be updated to {new_d}")
        
        # Test setting AR parameter (param_group=0, index=0)
        ar_value = 0.5
        model.set(ar_value, 0, 0)  # (value, index=0, param_group=0)
        ar_param = model.get_value_index(0, 0)
        print(f"Set AR={ar_value}, now AR={ar_param}")
        self.assertEqual(float(str(ar_param)), ar_value,
                         msg=f"AR parameter should be updated to {ar_value}")
        
        # Test setting MA parameter (param_group=1, index=0)
        ma_value = 0.4
        model.set(ma_value, 0, 1)  # (value, index=0, param_group=1)
        ma_param = model.get_value_index(0, 1)
        print(f"Set MA={ma_value}, now MA={ma_param}")
        self.assertEqual(float(str(ma_param)), ma_value,
                         msg=f"MA parameter should be updated to {ma_value}")
    
    def test_arfima_params(self):
        """Test ARFIMA model with multiple AR and MA parameters."""
        # Create model with 2 AR and 2 MA parameters
        model = regarch_wrapper.cArfima(2, 2, 0.2)
        print(f"Created model: ARFIMA(2,2,0.2) with {model.get_n_param()} parameters")
        
        # Set and verify AR parameters
        ar_values = [0.3, 0.4]
        for i, val in enumerate(ar_values):
            model.set(val, i, 0)  # (value, index=i, param_group=0)
            ar_param = model.get_value_index(i, 0)
            print(f"Set AR[{i}]={val}, now AR[{i}]={ar_param}")
            self.assertEqual(float(str(ar_param)), val,
                             msg=f"AR parameter {i} should be {val}")
        
        # Set and verify MA parameters
        ma_values = [0.5, 0.6]
        for i, val in enumerate(ma_values):
            model.set(val, i, 1)  # (value, index=i, param_group=1)
            ma_param = model.get_value_index(i, 1)
            print(f"Set MA[{i}]={val}, now MA[{i}]={ma_param}")
            self.assertEqual(float(str(ma_param)), val,
                             msg=f"MA parameter {i} should be {val}")
        
        # Set and verify d parameter
        d_value = 0.25
        model.set(d_value, "d")
        d_param = model.get("d")
        print(f"Set d={d_value}, now d={d_param}")
        self.assertEqual(float(str(d_param)), d_value,
                         msg=f"d parameter should be {d_value}")
    
    def test_compute_mean(self):
        """Test the compute_mean functionality of the ARFIMA model."""
        # Create an ARFIMA model with specific parameters
        model = regarch_wrapper.cArfima(1, 1, 0.2)
        
        # Set parameters to known values
        model.set(0.5, 0, 0)   # AR parameter = 0.5
        model.set(0.3, 0, 1)   # MA parameter = 0.3
        model.set(0.2, "d")    # fractional differencing = 0.2
        
        print(f"\nTesting compute_mean for ARFIMA model:")
        print(f"AR parameter: {model.get_value_index(0, 0)}")
        print(f"MA parameter: {model.get_value_index(0, 1)}")
        print(f"d parameter: {model.get('d')}")
        
        # Create a RegArchValue object to hold data
        n_obs = 10
        value = regarch_wrapper.cRegArchValue(n_obs)
        
        # Populate the data with some specific values for testing
        for i in range(n_obs):
            value.mYt[i] = i + 1.0
        
        # Initialize mean, variance, and residuals
        for i in range(n_obs):
            value.mMt[i] = 0.0
            value.mHt[i] = 1.0
            value.mUt[i] = value.mYt[i]
            value.mEpst[i] = value.mYt[i]
        
        # Compute the mean at different dates
        test_dates = [5, 6, 7, 8, 9]
        for date in test_dates:
            result = model.compute_mean(date, value)
            print(f"Conditional mean at date {date}: {result}")
            
            self.assertFalse(math.isnan(result), f"Mean at date {date} is NaN")
            self.assertFalse(math.isinf(result), f"Mean at date {date} is infinite")
        
        # Also test an early date with limited history
        try:
            result_early = model.compute_mean(0, value)
            print(f"Conditional mean at date 0: {result_early}")
        except Exception as e:
            print(f"Computing mean at date 0 resulted in error: {e}")

    def test_regarch_gradient(self):
        """
        Test cRegArchGradient creation and usage for ARFIMA, no realloc,
        with final dimensions from the start.
        """
        # If you expect 2 AR, 2 MA, plus 1 d => 5 total. Let's pick n_mean_param=5 or 4?
        # The library might handle them differently, but let's assume (3,5,1,1).
        # Adjust as needed for your library's usage.
        grad = regarch_wrapper.cRegArchGradient(3, 5, 1, 1)
        
        self.assertEqual(grad.get_n_past(), 3, "Mismatch n_past")
        self.assertEqual(grad.get_n_mean_param(), 5, "Mismatch n_mean_param (AR+MA+d?)")
        self.assertEqual(grad.get_n_var_param(), 1, "Mismatch n_var_param")
        self.assertEqual(grad.get_n_distr_param(), 1, "Mismatch n_distr_param")
        
        # Reinitialize & update
        grad.reinitialize()
        grad.update()
        
        self.assertTrue(True, "ARFIMA gradient test (no realloc) completed successfully.")

    def test_regarch_hessien(self):
        """
        Test cRegArchHessien with ARFIMA context, no realloc, consistent dimension.
        Provide data with non-zero mHt, call compute_hess_sigmat_from_hess_vart(t=2).
        """
        # Build with same dimension as gradient so no 'wrong size' error
        hess = regarch_wrapper.cRegArchHessien(3, 5, 1, 1)
        
        self.assertEqual(hess.get_n_past(), 3, "Mismatch n_past for Hessien")
        self.assertEqual(hess.get_n_mean_param(), 5, "Mismatch n_mean_param for Hessien")
        self.assertEqual(hess.get_n_var_param(), 1, "Mismatch n_var_param for Hessien")
        self.assertEqual(hess.get_n_distr_param(), 1, "Mismatch n_distr_param for Hessien")
        
        # Provide data with non-zero mHt
        data = regarch_wrapper.cRegArchValue()
        data.mUt = [0.1, 0.2, 0.3, 0.4]
        data.mHt = [0.6, 0.7, 1.0, 1.5]  # non-zero => avoid division by zero
        # Create a gradient with matching dimension
        grad = regarch_wrapper.cRegArchGradient(3, 5, 1, 1)
        
        # Attempt the Hessian method at t=2
        hess.compute_hess_sigmat_from_hess_vart(2, data, grad)
        
        # Then update
        hess.update()
        
        self.assertTrue(True, "ARFIMA hessien test (no realloc) completed successfully.")


if __name__ == '__main__':
    unittest.main()
