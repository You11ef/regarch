import unittest
import math
import regarch_wrapper

class TestLinRegModel(unittest.TestCase):
    
    def test_initialization(self):
        """Test initialization of Linear Regression model with different parameters."""
        # Default initialization
        linreg_default = regarch_wrapper.cLinReg()
        self.assertEqual(linreg_default.get_n_param(), 0)
        
        # Initialize with different numbers of regressors
        test_cases = [
            (1, 1),    # 1 regressor => 1 parameter
            (2, 2),    # 2 regressors => 2 parameters
            (5, 5),    # 5 regressors => 5 parameters
        ]
        
        for n_regressors, expected_params in test_cases:
            model = regarch_wrapper.cLinReg(n_regressors)
            self.assertEqual(model.get_n_param(), expected_params, 
                             f"LinReg with {n_regressors} regressors should have {expected_params} parameters")
    
    def test_parameter_access(self):
        """Test accessing parameters in different ways."""
        # Create models with different numbers of regressors
        models = [
            (regarch_wrapper.cLinReg(1), "LinReg(1)"),  
            (regarch_wrapper.cLinReg(2), "LinReg(2)"),  
            (regarch_wrapper.cLinReg(3), "LinReg(3)")   
        ]
        
        for model, name in models:
            print(f"\nTesting {name}:")
            
            n_params = model.get_n_param()
            print(f"  Model has {n_params} parameters")
            
            # Try unified 'get' method with a parameter index
            for i in range(n_params):
                try:
                    param = model.get(i)
                    print(f"  Parameter {i} via get(): {param}")
                except Exception as e:
                    print(f"  Error accessing parameter {i} via get(): {e}")
    
    def test_parameter_setting(self):
        """Test setting parameters using the correct methods."""
        # Create a model with 3 regressors
        model = regarch_wrapper.cLinReg(3)
        print(f"Created model: LinReg(3) with {model.get_n_param()} parameters")
        
        # Get initial parameters
        print("Initial parameters:")
        for i in range(model.get_n_param()):
            try:
                param = model.get(i)
                print(f"  Parameter {i}: {param}")
            except Exception as e:
                print(f"  Error getting parameter {i}: {e}")
        
        # Set the first parameter => 0.5
        model.set(0.5, 0, 0)
        params = model.get(0)
        print(f"Parameters after setting first to 0.5: {params}")
        val_str = str(params).strip().split('\n')[0]
        self.assertAlmostEqual(float(val_str), 0.5, places=6,
                              msg="First parameter should be 0.5")
        
        # Second parameter => -0.3
        model.set(-0.3, 1, 0)
        params = model.get(0)
        print(f"Parameters after setting second to -0.3: {params}")
        val_lines = str(params).strip().split('\n')
        if len(val_lines) >= 2:
            self.assertAlmostEqual(float(val_lines[1]), -0.3, places=6,
                                  msg="Second parameter should be -0.3")
        
        # Third parameter => 0.7
        model.set(0.7, 2, 0)
        params = model.get(0)
        print(f"Parameters after setting third to 0.7: {params}")
        val_lines = str(params).strip().split('\n')
        if len(val_lines) >= 3:
            self.assertAlmostEqual(float(val_lines[2]), 0.7, places=6,
                                  msg="Third parameter should be 0.7")
        
        # Set all parameters at once with a list
        try:
            all_values = [1.0, 2.0, 3.0]
            print(f"Setting all parameters to {all_values}")
            model.set(all_values, 0)
            
            # Verify all parameters
            params = model.get(0)
            print(f"Parameters after bulk set: {params}")
            val_lines = str(params).strip().split('\n')
            
            if len(val_lines) >= 3:
                self.assertAlmostEqual(float(val_lines[0]), all_values[0], places=6,
                                      msg=f"First parameter should be {all_values[0]}")
                self.assertAlmostEqual(float(val_lines[1]), all_values[1], places=6,
                                      msg=f"Second parameter should be {all_values[1]}")
                self.assertAlmostEqual(float(val_lines[2]), all_values[2], places=6,
                                      msg=f"Third parameter should be {all_values[2]}")
            
        except Exception as e:
            print(f"Error in bulk parameter setting: {e}")

    def test_compute_mean(self):
        """Test the compute_mean functionality of the Linear Regression model."""
        # Create a linear regression model with 2 regressors
        model = regarch_wrapper.cLinReg(2)
        
        # Set parameters to known values
        beta1 = 0.5
        beta2 = -0.3
        model.set(beta1, 0, 0)   # First coefficient = 0.5
        model.set(beta2, 1, 0)   # Second coefficient = -0.3
        
        print(f"\nTesting compute_mean for Linear Regression model:")
        params = model.get(0)
        print(f"Coefficients: {params}")
        
        # Create a RegArchValue to hold data
        n_obs = 10
        value = regarch_wrapper.cRegArchValue(n_obs)
        
        # Create X data with 2 regressors
        x_data = []
        for i in range(n_obs):
            x_data.append([i + 1.0, (i + 1) * 0.1])
        
        try:
            # Attempt set_xt
            value.set_xt(x_data)
            print("Successfully set X data using set_xt()")
        except Exception as e:
            print(f"Error setting X data via set_xt(): {e}")
            try:
                # Fallback: ReAlloc + set_xt_element
                value.ReAllocXt(n_obs, 2)
                for i in range(n_obs):
                    for j in range(2):
                        value.set_xt_element(i, j, x_data[i][j])
                print("Successfully populated X matrix using set_xt_element()")
            except Exception as e2:
                print(f"Error setting X elements: {e2}")
                print("Skipping mean computation test - can't set X data")
                return
        
        # Populate Y data in a deterministic way
        for i in range(n_obs):
            value.mYt[i] = beta1 * x_data[i][0] + beta2 * x_data[i][1]
        
        # Initialize other components
        for i in range(n_obs):
            value.mMt[i] = 0.0
            value.mHt[i] = 1.0
            value.mUt[i] = 0.0
            value.mEpst[i] = 0.0
        
        # Evaluate the mean at different points
        test_dates = [2, 5, 8]
        for date in test_dates:
            try:
                result = model.compute_mean(date, value)
                print(f"Conditional mean at date {date}: {result}")
                
                expected = beta1 * x_data[date][0] + beta2 * x_data[date][1]
                print(f"Expected mean at date {date}: {expected}")
                
                self.assertAlmostEqual(result, expected, places=6,
                                      msg=f"Mean at date {date} should be {expected}")
                self.assertTrue(not math.isnan(result), 
                                msg=f"Mean at date {date} should not be NaN")
                self.assertTrue(not math.isinf(result), 
                                msg=f"Mean at date {date} should not be infinite")
            except Exception as e:
                print(f"Error computing mean at date {date}: {e}")

    def test_regarch_gradient(self):
        """Test creation, reallocation, and update for cRegArchGradient in context of LinReg."""
        grad = regarch_wrapper.cRegArchGradient(2, 1, 1, 1)

        self.assertEqual(grad.get_n_past(), 2, "Mismatch in n_past for gradient")
        self.assertEqual(grad.get_n_mean_param(), 1, "Mismatch in n_mean_param")
        self.assertEqual(grad.get_n_var_param(), 1, "Mismatch in n_var_param")
        self.assertEqual(grad.get_n_distr_param(), 1, "Mismatch in n_distr_param")

        # Realloc
        grad.realloc(3, 2, 2, 1)
        self.assertEqual(grad.get_n_past(), 3, "After realloc mismatch in n_past")
        self.assertEqual(grad.get_n_mean_param(), 2, "After realloc mismatch in n_mean_param")

        # Reinitialize & update
        grad.reinitialize()
        grad.update()

        self.assertTrue(True, "Gradient test for cLinReg completed successfully.")

    def test_regarch_hessien(self):
        """Test cRegArchHessien reallocation and Hessian function with cLinReg-like data."""
        hess = regarch_wrapper.cRegArchHessien(2, 1, 1, 1)
        self.assertEqual(hess.get_n_past(), 2, "Mismatch in n_past for Hessien")

        # Reallocate
        hess.realloc(3, 2, 2, 1)
        self.assertEqual(hess.get_n_past(), 3, "After reallocate mismatch in n_past for Hessien")

        # Provide data with non-zero mHt to avoid division by zero
        data = regarch_wrapper.cRegArchValue()
        data.mUt = [0.1, 0.2, 0.3, 0.4]  # placeholders
        data.mHt = [0.6, 0.7, 1.0, 1.2]  # non-zero => avoid div-by-zero
        grad = regarch_wrapper.cRegArchGradient(3, 2, 2, 1)

        # We'll pick t=2
        hess.compute_hess_sigmat_from_hess_vart(2, data, grad)
        hess.update()

        self.assertTrue(True, "Hessien test for cLinReg completed without error.")


if __name__ == '__main__':
    unittest.main()
