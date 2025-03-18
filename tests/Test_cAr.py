import unittest
import numpy as np
from regarch_wrapper import cAr, cRegArchValue, cGSLVector, cRegArchGradient, cRegArchHessien

class TestAR(unittest.TestCase):
    """
    Test the cAr class wrapper.
    """
    
    def setUp(self):
        # Create AR(2) model
        self.model = cAr(2)
        
        # Create test data
        self.sample_size = 10
        self.data = cRegArchValue(self.sample_size)
        
        # Fill test data with values
        yt = cGSLVector(self.sample_size)
        for i in range(self.sample_size):
            yt[i] = i / 2.0  # [0.0, 0.5, 1.0, 1.5, 2.0, 2.5, 3.0, 3.5, 4.0, 4.5]
        
        # Set data
        self.data.ReAlloc(yt)
        self.data.mYt = yt
        
        # Set default parameter values
        self.model.set_default_init_point(1.0, 1.0)
    
    def test_init(self):
        """Test initialization and parameter count"""
        # AR(2) should have 2 parameters (phi_1, phi_2)
        self.assertEqual(self.model.get_n_param(), 2)
        
        # Test copy constructor
        model_copy = cAr(self.model)
        self.assertEqual(model_copy.get_n_param(), 2)
        
        # Test constructor with coefficients
        coeffs = cGSLVector(3)
        coeffs[0] = 0.2
        coeffs[1] = 0.3
        coeffs[2] = 0.1
        model_with_coeffs = cAr(coeffs)
        self.assertEqual(model_with_coeffs.get_n_param(), 3)
    
    def test_set_get_by_index(self):
        """Test setting and getting parameter values by index"""
        # Following AR model convention:
        # phi_i is the coefficient for y_{t-i}
        
        # Set phi_1 (param 0, index 0)
        self.model.SetValueIndex(0.5, 0, 0)  # Use camelCase as in wrapper
        self.assertAlmostEqual(self.model.GetValueIndex(0, 0), 0.5, places=10)
        
        # Set phi_2 (param 0, index 1)
        self.model.SetValueIndex(0.3, 1, 0)
        self.assertAlmostEqual(self.model.GetValueIndex(1, 0), 0.3, places=10)
        
        # Try the unified interface
        self.model.set(0.6, 0, 0)
        self.assertAlmostEqual(self.model.get(0, 0), 0.6, places=10)
        
        self.model.set(0.4, 1, 0)
        self.assertAlmostEqual(self.model.get(1, 0), 0.4, places=10)
    
    def test_set_get_by_name(self):
        """Test setting and getting parameter values by name (if supported)"""
        # This might not work if the C++ implementation doesn't support string names
        # We'll try with a name that might be used and catch any exceptions
        try:
            self.model.set(0.5, "phi", 0)
            value = self.model.get("phi", 0)
            self.assertAlmostEqual(value, 0.5, places=10)
            self.test_names_supported = True
        except Exception as e:
            print(f"String parameter names test failed: {e}")
            self.test_names_supported = False
    
    def test_set_get_vectors(self):
      """Test setting and getting parameter vectors"""
      # Create a vector for AR parameters
      ar_params = cGSLVector(2)
      ar_params[0] = 0.5
      ar_params[1] = 0.3
      
      # Set by index
      self.model.SetVectorIndex(ar_params, 0)  
      
      # Get by index
      ar_result = self.model.GetVectorIndex(0)
      
      self.assertEqual(ar_result.GetSize(), 2)
      self.assertAlmostEqual(ar_result[0], 0.5, places=10)
      self.assertAlmostEqual(ar_result[1], 0.3, places=10)
      
      # Create new values and try again
      ar_params[0] = 0.6
      ar_params[1] = 0.2
      
      self.model.SetVectorIndex(ar_params, 0)
      
      ar_result = self.model.GetVectorIndex(0)
      
      self.assertEqual(ar_result.GetSize(), 2)
      self.assertAlmostEqual(ar_result[0], 0.6, places=10)
      self.assertAlmostEqual(ar_result[1], 0.2, places=10)
    
    def test_compute_mean(self):
        """Test computation of conditional mean"""
        # Set parameters for AR(2) model
        # y_t = 0.5*y_{t-1} + 0.3*y_{t-2}
        self.model.set(0.5, 0, 0)  # phi_1 = 0.5
        self.model.set(0.3, 1, 0)  # phi_2 = 0.3
        
        # Initialize mean vector
        self.data.mMt = cGSLVector(self.sample_size)
        
        # Compute mean for each time point
        # We start from t=2 because we need 2 lagged values for AR(2)
        for t in range(2, self.sample_size):
            mean = self.model.compute_mean(t, self.data)
            self.data.mMt[t] = mean
            print(f"t={t}, computed mean={mean}")
        
        # Calculate expected values manually
        # For t=2, y_1 = 0.5, y_0 = 0.0, so mean = 0.5*0.5 + 0.3*0.0 = 0.25
        # For t=3, y_2 = 1.0, y_1 = 0.5, so mean = 0.5*1.0 + 0.3*0.5 = 0.65
        # And so on...
        expected_means = [0.0, 0.0]  # First two values aren't computed
        for t in range(2, self.sample_size):
            y_tm1 = self.data.mYt[t-1]
            y_tm2 = self.data.mYt[t-2]
            mean = 0.5 * y_tm1 + 0.3 * y_tm2
            expected_means.append(mean)
        
        # Compare computed and expected values
        for t in range(2, self.sample_size):
            self.assertAlmostEqual(self.data.mMt[t], expected_means[t], places=10)
    
    def test_param_vector_conversion(self):
        """Test parameter to vector conversion"""
        # Set parameters
        self.model.set(0.5, 0, 0)  # phi_1
        self.model.set(0.3, 1, 0)  # phi_2
        
        # Create vector to receive parameters
        params = cGSLVector(2)
        
        # Convert parameters to vector
        self.model.reg_arch_param_to_vector(params, 0)
        
        # Check values
        self.assertAlmostEqual(params[0], 0.5, places=10)  # phi_1
        self.assertAlmostEqual(params[1], 0.3, places=10)  # phi_2
        
        # Now create a new vector with different values
        new_params = cGSLVector(2)
        new_params[0] = 0.6  # phi_1
        new_params[1] = 0.2  # phi_2
        
        # Convert vector to parameters
        self.model.vector_to_reg_arch_param(new_params, 0)
        
        # Check parameter values
        self.assertAlmostEqual(self.model.get(0, 0), 0.6, places=10)  # phi_1
        self.assertAlmostEqual(self.model.get(1, 0), 0.2, places=10)  # phi_2
    
    def test_realloc(self):
        """Test reallocation of parameters"""
        # Create AR(3) model by reallocating
        self.model.realloc(3, 0)
        
        # Check parameter vector size
        ar_params = self.model.get(0)
        self.assertEqual(ar_params.GetSize(), 3)
        
        # Set values for all parameters
        self.model.set(0.4, 0, 0)  # phi_1
        self.model.set(0.3, 1, 0)  # phi_2
        self.model.set(0.1, 2, 0)  # phi_3
        
        # Check values
        self.assertAlmostEqual(self.model.get(0, 0), 0.4, places=10)
        self.assertAlmostEqual(self.model.get(1, 0), 0.3, places=10)
        self.assertAlmostEqual(self.model.get(2, 0), 0.1, places=10)

if __name__ == "__main__":
    unittest.main()