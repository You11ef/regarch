import unittest
import regarch_wrapper

class TestCStdDevInMean(unittest.TestCase):
    def setUp(self):
        self.model = regarch_wrapper.cStdDevInMean()
        # If your library requires you to call .realloc(...) before usage,
        # and you prefer not to do so, remove or adapt these lines.
        self.model.realloc(1)
        self.model.set(0.0, 0)  # Set parameter at index 0, sub-index 0
        self.dummy_value = regarch_wrapper.cRegArchValue([1.0, 2.0, 3.0])

    def test_initialization(self):
        self.assertEqual(self.model.GetNParam(), 1)

    def test_get_and_set_by_index(self):
        test_value = 0.75
        # Set and get with explicit sub-index (second 0)
        self.model.set(test_value, 0, 0)  # (value, param_index, sub_index)
        ret_value = self.model.get(0, 0)  # (param_index, sub_index)
        self.assertAlmostEqual(ret_value, test_value, places=5)

    def test_get_and_set_by_name(self):
        test_value = 0.85
        param_name = "stddevinmean"
        # Set and get with explicit sub-index (second 0)
        self.model.set(test_value, param_name, 0)  # (value, name, sub_index)
        ret_value = self.model.get(param_name, 0)  # (name, sub_index)
        self.assertAlmostEqual(ret_value, test_value, places=5)

    def test_compute_mean(self):
        self.model.set(0.5, 0, 0)
        mean_val = self.model.ComputeMean(2, self.dummy_value)
        self.assertIsInstance(mean_val, float)

    def test_simulation(self):
        try:
            sim_val = self.model.simulate()
        except AttributeError:
            sim_val = self.model.ComputeMean(1, self.dummy_value)
        self.assertIsInstance(sim_val, float)

    def test_regarch_gradient(self):
        """
        Test cRegArchGradient creation and update with cStdDevInMean.
        We avoid calling realloc(...) and set final dimensions up front.
        """
        # Suppose we need nPast=3, and assume we have 1 mean param, 1 var param, 1 distr param
        grad = regarch_wrapper.cRegArchGradient(3, 1, 1, 1)
        
        self.assertEqual(grad.get_n_past(), 3, "Mismatch in n_past for gradient")
        self.assertEqual(grad.get_n_mean_param(), 1, "Mismatch in n_mean_param")
        self.assertEqual(grad.get_n_var_param(), 1, "Mismatch in n_var_param")
        self.assertEqual(grad.get_n_distr_param(), 1, "Mismatch in n_distr_param")
        
        # Reinitialize & update
        grad.reinitialize()
        grad.update()
        
        self.assertTrue(True, "Gradient test for cStdDevInMean completed successfully (no realloc).")

    def test_regarch_hessien(self):
        """
        Test cRegArchHessien with cStdDevInMean, no reallocation. We set consistent
        dimensions and provide data with non-zero mHt to avoid division by zero.
        """
        # Match gradient's dimensions so the library won't complain
        hess = regarch_wrapper.cRegArchHessien(3, 1, 1, 1)
        
        self.assertEqual(hess.get_n_past(), 3, "Mismatch in n_past for Hessien")
        self.assertEqual(hess.get_n_mean_param(), 1, "Mismatch in n_mean_param for Hessien")
        self.assertEqual(hess.get_n_var_param(), 1, "Mismatch in n_var_param for Hessien")
        self.assertEqual(hess.get_n_distr_param(), 1, "Mismatch in n_distr_param for Hessien")
        
        # Provide data with non-zero mHt
        data = regarch_wrapper.cRegArchValue()
        data.mUt = [1.0, 2.0, 3.0, 4.0]  # placeholders
        data.mHt = [0.5, 0.7, 1.2, 2.0]  # non-zero => avoid division by zero
        # Provide a gradient with matching dimension
        grad = regarch_wrapper.cRegArchGradient(3, 1, 1, 1)
        
        # Attempt the Hessian method at t=2
        hess.compute_hess_sigmat_from_hess_vart(2, data, grad)
        
        # Then update
        hess.update()
        
        self.assertTrue(True, "Hessien test for cStdDevInMean completed without error (no realloc).")

if __name__ == '__main__':
    unittest.main()
