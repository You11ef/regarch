import unittest
import numpy as np
from regarch_wrapper import cAbstCondVar, eCondVarEnum, cRegArchValue, cGSLVector, cGSLMatrix

class MockCondVar(cAbstCondVar):
    """
    A mock implementation of cAbstCondVar for testing the wrapper.
    This is a minimal implementation that does just enough to test the interface.
    """
    
    def __init__(self):
        cAbstCondVar.__init__(self, eCondVarEnum.eNotKnown)
        self.param_value = 0.0
        self.param_vector = cGSLVector(1)
        self.param_vector[0] = self.param_value
    
    # Required interface methods
    def Delete(self):
        pass
    
    def PrintImpl(self):
        print(f"MockCondVar: param_value = {self.param_value}")
    
    # Set methods
    def SetDoubleIndex(self, value, index, param_num=0):
        if index == 0:  # We only have one parameter
            self.param_value = value
            self.param_vector[0] = value
    
    def SetVectorIndex(self, values, param_num=0):
        if values.GetSize() > 0:
            self.param_value = values[0]
            self.param_vector[0] = values[0]
    
    def SetDoubleName(self, value, param_name, index=0):
        if param_name == "omega":
            self.param_value = value
            self.param_vector[0] = value
    
    def SetVectorName(self, values, param_name):
        if param_name == "omega" and values.GetSize() > 0:
            self.param_value = values[0]
            self.param_vector[0] = values[0]
    
    # Get methods
    def GetIndex(self, index, param_num):
        if index == 0:
            return self.param_value
        return 0.0
    
    def GetVector(self, param_num):
        return self.param_vector
    
    def GetNameIndex(self, param_name, index):
        if param_name == "omega" and index == 0:
            return self.param_value
        return 0.0
    
    def GetName(self, param_name):
        if param_name == "omega":
            return self.param_vector
        # Return empty vector for unknown parameter names
        empty_vec = cGSLVector(0)
        return empty_vec
    
    # ReAlloc methods
    def ReAllocSize(self, size, param_num=0):
        # Our mock only has one parameter, so we don't need to do anything here
        pass
    
    def ReAllocVector(self, vector, param_num=0):
        # Our mock only has one parameter, so we don't need to do anything here
        pass
    
    def ReAllocProxyVarParameters(self, old_n_param=0):
        pass
    
    def SyncSizeParam(self, num_param):
        pass
    
    def UpdateProxyVarParameters(self):
        pass
    
    # Computation methods
    def ComputeVar(self, date, data):
        # For a constant variance model, var is simply the parameter value
        return self.param_value
    
    def GetNParam(self):
        return 1  # Our mock has 1 parameter
    
    def GetNLags(self):
        return 0  # Constant model has no lags
    
    def ComputeGrad(self, date, data, grad_data, residuals=None):
        # For simplicity, just set the gradient to 1
        grad_data.current_grad_var[0] = 1.0
    
    def RegArchParamToVector(self, dest_vect, index=0):
        dest_vect[index] = self.param_value
    
    def VectorToRegArchParam(self, src_vect, index=0):
        self.param_value = src_vect[index]
        self.param_vector[0] = src_vect[index]
    
    def SetDefaultInitPointMeanVar(self, mean, var):
        self.param_value = var  # For variance, we use the variance as init value
        self.param_vector[0] = var
    
    def SetDefaultInitPointValue(self, value):
        # Use the variance of the data as initial value
        n = value.mYt.GetSize()
        if n > 0:
            mean = 0.0
            for i in range(n):
                mean += value.mYt[i]
            mean /= n
            
            var = 0.0
            for i in range(n):
                var += (value.mYt[i] - mean) ** 2
            var /= n
            
            self.param_value = var
            self.param_vector[0] = var
    
    def ComputeHess(self, date, data, grad_data, hess_data, residuals=None):
        # For simplicity, just set the hessian to 0
        pass
    
    def ComputeGradAndHess(self, date, data, grad_data, hess_data, residuals=None):
        # Call the individual methods
        self.ComputeGrad(date, data, grad_data, residuals)
        self.ComputeHess(date, data, grad_data, hess_data, residuals)
    
    def GetParamNameChar(self, index, name_ptr):
        # This is difficult to implement in Python
        pass
    
    def GetParamNameString(self, index, name_array):
        # Fill the name array with parameter names
        if index == 0:
            name_array[0] = "omega"


class TestMockCondVar(unittest.TestCase):
    """
    Test our MockCondVar implementation directly.
    This helps isolate issues between our mock and the wrapper.
    """
    
    def setUp(self):
        self.model = MockCondVar()
        
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
        self.model.SetVectorIndex(vec, 0)
        self.assertEqual(self.model.param_value, 3.75)
        
        # Reset
        self.model.param_value = 0.0
        self.model.param_vector[0] = 0.0
        
        # Set param via vector by name
        self.model.SetVectorName(vec, "omega")
        self.assertEqual(self.model.param_value, 3.75)
    
    def test_scalar_methods(self):
        """Test that our scalar operations work"""
        # Set param via scalar by index
        self.model.SetDoubleIndex(4.25, 0, 0)
        self.assertEqual(self.model.param_value, 4.25)
        
        # Reset
        self.model.param_value = 0.0
        self.model.param_vector[0] = 0.0
        
        # Set param via scalar by name
        self.model.SetDoubleName(4.25, "omega", 0)
        self.assertEqual(self.model.param_value, 4.25)
    
    def test_compute_method(self):
        """Test compute method"""
        self.model.param_value = 5.0
        var = self.model.ComputeVar(0, self.data)
        self.assertEqual(var, 5.0)
    
    def test_default_init_point_methods(self):
        """Test setting default init points"""
        # Test with mean and variance
        self.model.SetDefaultInitPointMeanVar(0.0, 5.5)
        self.assertEqual(self.model.param_value, 5.5)
        
        # Reset
        self.model.param_value = 0.0
        self.model.param_vector[0] = 0.0
        
        # Test with RegArchValue
        self.model.SetDefaultInitPointValue(self.data)
        # Expected variance of [1.0, 2.0, 3.0, 4.0, 5.0] is 2.0
        # (mean is 3.0, so variance is (1-3)^2 + (2-3)^2 + (3-3)^2 + (4-3)^2 + (5-3)^2) / 5 = 2.0
        self.assertAlmostEqual(self.model.param_value, 2.0, places=10)


if __name__ == "__main__":
    unittest.main()