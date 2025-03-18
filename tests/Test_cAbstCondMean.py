import unittest
import numpy as np
from regarch_wrapper import cAbstCondMean, CondMeanEnum, cRegArchValue, cGSLVector, cGSLMatrix

class MockCondMean(cAbstCondMean):
    """
    A mock implementation of cAbstCondMean for testing the wrapper.
    This is a minimal implementation that does just enough to test the interface.
    """
    
    def __init__(self):
        cAbstCondMean.__init__(self, CondMeanEnum.eConst)
        self.param_value = 0.0
        self.param_vector = cGSLVector(1)
        self.param_vector[0] = self.param_value
    
    # Required interface methods
    def delete(self):
        pass
    
    def print(self):
        print(f"MockCondMean: param_value = {self.param_value}")
    
    # Set methods
    def set_value_by_index(self, value, index, param_num=0):
        if index == 0:  # We only have one parameter
            self.param_value = value
            self.param_vector[0] = value
    
    def set_vector_by_index(self, values, param_num=0):
        if values.GetSize() > 0:
            self.param_value = values[0]
            self.param_vector[0] = values[0]
    
    def set_value_by_name(self, value, param_name, index=0):
        if param_name == "constant":
            self.param_value = value
            self.param_vector[0] = value
    
    def set_vector_by_name(self, values, param_name):
        if param_name == "constant" and values.GetSize() > 0:
            self.param_value = values[0]
            self.param_vector[0] = values[0]
    
    # Get methods
    def get_value_by_index(self, index, param_num):
        if index == 0:
            return self.param_value
        return 0.0
    
    def get_vector_by_index(self, param_num):
        return self.param_vector
    
    def get_value_by_name(self, param_name, index):
        if param_name == "constant" and index == 0:
            return self.param_value
        return 0.0
    
    def get_vector_by_name(self, param_name):
        if param_name == "constant":
            return self.param_vector
        # Return empty vector for unknown parameter names
        empty_vec = cGSLVector(0)
        return empty_vec
    
    # ReAlloc methods
    def realloc_by_size(self, size, param_num=0):
        # Our mock only has one parameter, so we don't need to do anything here
        pass
    
    def realloc_by_vector(self, vector, param_num=0):
        # Our mock only has one parameter, so we don't need to do anything here
        pass
    
    def realloc_proxy_mean_parameters(self, old_n_param=0):
        pass
    
    def update_proxy_mean_parameters(self):
        pass
    
    # Computation methods
    def compute_mean(self, date, data):
        # For a constant model, mean is simply the constant value
        return self.param_value
    
    def get_n_param(self):
        return 1  # Our mock has one parameter
    
    def get_n_lags(self):
        return 0  # Constant model has no lags
    
    def compute_grad(self, date, data, grad_data, beg_index):
        # Gradient of constant model is 1 with respect to the constant parameter
        pass  # Skip for now
    
    def reg_arch_param_to_vector(self, dest_vect, index=0):
        dest_vect[index] = self.param_value
    
    def vector_to_reg_arch_param(self, src_vect, index=0):
        self.param_value = src_vect[index]
        self.param_vector[0] = src_vect[index]
    
    def set_default_init_point_mean_var(self, mean, var):
        self.param_value = mean
        self.param_vector[0] = mean
    
    def set_default_init_point_value(self, value):
        # Use the mean of the data as initial constant
        total = 0.0
        n = value.mYt.GetSize()
        if n > 0:
            for i in range(n):
                total += value.mYt[i]
            self.param_value = total / n
            self.param_vector[0] = self.param_value
    
    def compute_hess(self, date, data, grad_data, hess_data, beg_index):
        # Hessian of constant model is 0 (second derivative)
        pass
    
    def compute_grad_and_hess(self, date, data, grad_data, hess_data, beg_index):
        # Compute both in one go for efficiency
        pass
    
    def get_param_name_string(self, index, name_array):
        # Fill the name array with parameter names
        if index == 0:
            name_array[0] = "constant"


class TestMockCondMean(unittest.TestCase):
    """
    Test our MockCondMean implementation directly.
    This helps isolate issues between our mock and the wrapper.
    """
    
    def setUp(self):
        self.model = MockCondMean()
        
        # Create test data
        self.sample_size = 5
        self.data = cRegArchValue(self.sample_size)
        
        # Fill test data with values
        yt = cGSLVector(self.sample_size)
        for i in range(self.sample_size):
            yt[i] = i + 1.0  # [1.0, 2.0, 3.0, 4.0, 5.0]
        
        self.data.ReAlloc(yt)
    
    def test_model_init(self):
        """Test that our model initializes correctly"""
        self.assertEqual(self.model.param_value, 0.0)
        self.assertEqual(self.model.param_vector.GetSize(), 1)
        self.assertEqual(self.model.param_vector[0], 0.0)
    
    def test_direct_set_get(self):
        """Test direct attribute setting and getting"""
        self.model.param_value = 2.5
        self.model.param_vector[0] = 2.5
        
        self.assertEqual(self.model.param_value, 2.5)
        self.assertEqual(self.model.param_vector[0], 2.5)
    
    def test_vector_methods(self):
        """Test that our vector operations work"""
        # Create a vector
        vec = cGSLVector(1)
        vec[0] = 3.75
        
        # Set param via vector by index
        self.model.set_vector_by_index(vec, 0)
        self.assertEqual(self.model.param_value, 3.75)
        
        # Reset
        self.model.param_value = 0.0
        self.model.param_vector[0] = 0.0
        
        # Set param via vector by name
        self.model.set_vector_by_name(vec, "constant")
        self.assertEqual(self.model.param_value, 3.75)
    
    def test_scalar_methods(self):
        """Test that our scalar operations work"""
        # Set param via scalar by index
        self.model.set_value_by_index(4.25, 0, 0)
        self.assertEqual(self.model.param_value, 4.25)
        
        # Reset
        self.model.param_value = 0.0
        self.model.param_vector[0] = 0.0
        
        # Set param via scalar by name
        self.model.set_value_by_name(4.25, "constant", 0)
        self.assertEqual(self.model.param_value, 4.25)
    
    def test_compute_method(self):
        """Test compute method"""
        self.model.param_value = 5.0
        mean = self.model.compute_mean(0, self.data)
        self.assertEqual(mean, 5.0)
    
    def test_default_init_point_methods(self):
        """Test setting default init points"""
        # Test with mean and variance
        self.model.set_default_init_point_mean_var(5.5, 1.0)
        self.assertEqual(self.model.param_value, 5.5)
        
        # Reset
        self.model.param_value = 0.0
        self.model.param_vector[0] = 0.0
        
        # Test with RegArchValue
        self.model.set_default_init_point_value(self.data)
        # Expected mean of [1.0, 2.0, 3.0, 4.0, 5.0] is 3.0
        self.assertEqual(self.model.param_value, 3.0)


if __name__ == "__main__":
    unittest.main()