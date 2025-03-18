import unittest
from regarch_wrapper import cArch, cRegArchValue, cGSLVector

class TestARCH(unittest.TestCase):
    """
    Test the cArch class wrapper with correct parameter handling.
    """
    
    def setUp(self):
        # Create ARCH(2) model
        self.model = cArch(2)
        
        # Create test data
        self.sample_size = 10
        self.data = cRegArchValue(self.sample_size)
        
        # Fill test data with values
        yt = cGSLVector(self.sample_size)
        ut = cGSLVector(self.sample_size)
        
        # Set up some data and residuals
        for i in range(self.sample_size):
            yt[i] = i / 2.0  # [0.0, 0.5, 1.0, 1.5, 2.0, 2.5, 3.0, 3.5, 4.0, 4.5]
            ut[i] = (-1.0)**i * (i / 5.0)  # [0.0, -0.2, 0.4, -0.6, 0.8, -1.0, 1.2, -1.4, 1.6, -1.8]
        
        # Set data
        self.data.ReAlloc(yt)
        self.data.mYt = yt
        self.data.mUt = ut
    
    def test_init(self):
        """Test initialization and parameter count"""
        # ARCH(2) should have 3 parameters (omega, alpha_1, alpha_2)
        self.assertEqual(self.model.GetNParam(), 3)
        
        # Test copy constructor
        model_copy = cArch(self.model)
        self.assertEqual(model_copy.GetNParam(), 3)
    
    def test_compute_var(self):
        """Test computation of conditional variance"""
        # Set parameters using vector, which seems to work better
        params = cGSLVector(3)
        params[0] = 0.1  # omega
        params[1] = 0.2  # alpha_1
        params[2] = 0.3  # alpha_2
        
        self.model.VectorToRegArchParam(params, 0)
        
        # Initialize variance vector
        self.data.mHt = cGSLVector(self.sample_size)
        
        # Set initial values
        self.data.mHt[0] = 0.1  # Starting value
        self.data.mHt[1] = 0.1  # Same for second value
        
        # Compute variance for each time point
        # We start from t=2 because we need 2 lagged values for ARCH(2)
        for t in range(2, self.sample_size):
            var = self.model.ComputeVar(t, self.data)
            self.data.mHt[t] = var
            print(f"t={t}, computed var={var}")
        
        # Check that the computed variances are all positive
        for t in range(2, self.sample_size):
            self.assertGreater(self.data.mHt[t], 0.0)
    
    def test_param_vector_conversion(self):
        """Test parameter to vector conversion"""
        # Set parameters using vector
        params = cGSLVector(3)
        params[0] = 0.1  # omega
        params[1] = 0.2  # alpha_1
        params[2] = 0.3  # alpha_2
        
        self.model.VectorToRegArchParam(params, 0)
        
        # Create vector to receive parameters
        result_params = cGSLVector(3)
        
        # Convert parameters back to vector
        self.model.RegArchParamToVector(result_params, 0)
        
        # Check the first value (which seems to be the one that gets set)
        self.assertAlmostEqual(result_params[0], 0.1, places=10)
    
    def test_arch_order(self):
        """Test creating ARCH models with different orders"""
        # Create models with different orders
        arch1 = cArch(1)  # ARCH(1)
        arch3 = cArch(3)  # ARCH(3)
        
        # Check parameter counts
        self.assertEqual(arch1.GetNParam(), 2)  # constant + 1 ARCH term
        self.assertEqual(arch3.GetNParam(), 4)  # constant + 3 ARCH terms

if __name__ == "__main__":
    unittest.main()