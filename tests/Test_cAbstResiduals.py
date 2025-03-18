import unittest
import numpy as np
from regarch_wrapper import cAbstResiduals, eDistrTypeEnum, cRegArchValue, cGSLVector, cGSLMatrix, cNumericDerivative

class MockResiduals(cAbstResiduals):
    """
    A mock implementation of cAbstResiduals for testing the wrapper.
    This implements a simple normal distribution with fixed parameters.
    """
    
    def __init__(self):
        # Initialize with Normal distribution, no parameters (we'll add them manually)
        cAbstResiduals.__init__(self, eDistrTypeEnum.eNormal, None, False)
        
        # For a standard normal distribution
        self.mean = 0.0
        self.std_dev = 1.0
        
        # Create a parameter vector for distribution parameters
        self.distr_param = cGSLVector(2)
        self.distr_param[0] = self.mean
        self.distr_param[1] = self.std_dev
    
    def PrintImpl(self):
        print(f"MockResiduals: Normal(mean={self.mean}, std_dev={self.std_dev})")
    
    def Generate(self, n_sample, yt):
        # Simple random normal generation - using fixed seed for reproducibility
        # In a real implementation, we would use the GSL random generator
        import random
        random.seed(42)
        for i in range(n_sample):
            # Box-Muller transform for normal distribution
            u1 = random.random()
            u2 = random.random()
            z = np.sqrt(-2.0 * np.log(u1)) * np.cos(2.0 * np.pi * u2)
            yt[i] = self.mean + self.std_dev * z
    
    def LogDensity(self, x):
        # Log density of normal distribution: -0.5*log(2π) - log(σ) - 0.5*((x-μ)/σ)²
        return -0.5 * np.log(2.0 * np.pi) - np.log(self.std_dev) - 0.5 * ((x - self.mean) / self.std_dev) ** 2
    
    def GetNParam(self):
        return 2  # mean and std_dev
    
    def DiffLogDensity(self, x):
        # First derivative of log density with respect to x: -(x-μ)/σ²
        return -(x - self.mean) / (self.std_dev ** 2)
    
    def ComputeGrad(self, date, data, grad_data):
        # For normal distribution:
        # d(log f)/dμ = (x-μ)/σ²
        # d(log f)/dσ = (x-μ)²/σ³ - 1/σ
        x = data.mUt[date]
        grad_data.mCurrentDiffLogDensity = self.DiffLogDensity(x)
        # Assuming mCurrentGradLogDens is the gradient with respect to distribution parameters
        if grad_data.mCurrentGradLogDens.GetSize() >= 2:
            grad_data.mCurrentGradLogDens[0] = (x - self.mean) / (self.std_dev ** 2)
            grad_data.mCurrentGradLogDens[1] = ((x - self.mean) ** 2) / (self.std_dev ** 3) - 1.0 / self.std_dev
    
    def RegArchParamToVector(self, dest_vect, index):
        dest_vect[index] = self.mean
        dest_vect[index + 1] = self.std_dev
    
    def VectorToRegArchParam(self, src_vect, index=0):
        self.mean = src_vect[index]
        self.std_dev = src_vect[index + 1]
        self.distr_param[0] = self.mean
        self.distr_param[1] = self.std_dev
    
    def ComputeEspAbsEps(self):
        # For normal distribution, E[|X|] = std_dev * sqrt(2/π)
        return self.std_dev * np.sqrt(2.0 / np.pi)
    
    def ComputeGradBetaEspAbsEps(self, grad):
        # d/dμ E[|X|] = 0
        # d/dσ E[|X|] = sqrt(2/π)
        if grad.GetSize() >= 2:
            grad[0] = 0.0
            grad[1] = np.sqrt(2.0 / np.pi)
    
    def ComputeHessBetaEspAbsEps(self, hess):
        # All second derivatives are 0
        if hess.GetNRow() >= 2 and hess.GetNCol() >= 2:
            hess.Set(0, 0, 0.0)
            hess.Set(0, 1, 0.0)
            hess.Set(1, 0, 0.0)
            hess.Set(1, 1, 0.0)
    
    def SetDefaultInitPoint(self):
        # Default values for normal distribution
        self.mean = 0.0
        self.std_dev = 1.0
        self.distr_param[0] = self.mean
        self.distr_param[1] = self.std_dev
    
    def Diff2LogDensity(self, x):
        # Second derivative of log density with respect to x: -1/σ²
        return -1.0 / (self.std_dev ** 2)
    
    def GradDiffLogDensity(self, x, distr_param, grad):
        # Gradient of the first derivative with respect to distribution parameters
        # d²(log f)/dxdμ = 1/σ²
        # d²(log f)/dxdσ = 2(x-μ)/σ³
        if grad.GetSize() >= 2:
            mean = distr_param[0]
            std_dev = distr_param[1]
            grad[0] = 1.0 / (std_dev ** 2)
            grad[1] = 2.0 * (x - mean) / (std_dev ** 3)
    
    def ComputeHess(self, date, data, grad_data, hess_data):
        # For normal distribution, the Hessian of log density wrt parameters is:
        # d²(log f)/dμ² = -1/σ²
        # d²(log f)/dμdσ = -2(x-μ)/σ³
        # d²(log f)/dσ² = -3(x-μ)²/σ⁴ + 2/σ²
        x = data.mUt[date]
        if hess_data.mCurrentHessDens.GetNRow() >= 2 and hess_data.mCurrentHessDens.GetNCol() >= 2:
            hess_data.mCurrentHessDens.Set(0, 0, -1.0 / (self.std_dev ** 2))
            hess_data.mCurrentHessDens.Set(0, 1, -2.0 * (x - self.mean) / (self.std_dev ** 3))
            hess_data.mCurrentHessDens.Set(1, 0, -2.0 * (x - self.mean) / (self.std_dev ** 3))
            hess_data.mCurrentHessDens.Set(1, 1, -3.0 * ((x - self.mean) ** 2) / (self.std_dev ** 4) + 2.0 / (self.std_dev ** 2))
    
    def ComputeGradAndHess(self, date, data, grad_data, hess_data):
        # Compute both gradient and Hessian in one go
        self.ComputeGrad(date, data, grad_data)
        self.ComputeHess(date, data, grad_data, hess_data)
    
    def GetParamNameChar(self, index, name_ptr):
        # This is difficult to implement in Python
        pass
    
    def GetParamNameString(self, index, name_array):
        if index == 0:
            name_array[0] = "mean"
        elif index == 1:
            name_array[0] = "std_dev"


class TestMockResiduals(unittest.TestCase):
    """
    Test our MockResiduals implementation directly.
    This helps isolate issues between our mock and the wrapper.
    """
    
    def setUp(self):
        self.residuals = MockResiduals()
        
        # Create test data
        self.sample_size = 5
        self.data = cRegArchValue(self.sample_size)
        
        # Fill test data with values
        yt = cGSLVector(self.sample_size)
        for i in range(self.sample_size):
            yt[i] = i - 2.0  # [-2.0, -1.0, 0.0, 1.0, 2.0]
        
        self.data.ReAlloc(yt)
        
        # Create residuals (Ut) for the test data
        self.data.mUt = cGSLVector(self.sample_size)
        for i in range(self.sample_size):
            self.data.mUt[i] = yt[i]  # Using same values as yt for simplicity
    
    def test_init(self):
        """Test that our residuals model initializes correctly"""
        self.assertEqual(self.residuals.mean, 0.0)
        self.assertEqual(self.residuals.std_dev, 1.0)
        self.assertEqual(self.residuals.distr_param.GetSize(), 2)
        self.assertEqual(self.residuals.distr_param[0], 0.0)
        self.assertEqual(self.residuals.distr_param[1], 1.0)
        self.assertEqual(self.residuals.GetDistrType(), eDistrTypeEnum.eNormal)
    
    def test_log_density(self):
        """Test log density calculation"""
        # For standard normal, log density at x=0 is -0.5*log(2π)
        log_dens_at_zero = self.residuals.LogDensity(0.0)
        expected_log_dens = -0.5 * np.log(2.0 * np.pi)
        self.assertAlmostEqual(log_dens_at_zero, expected_log_dens, places=10)
        
        # For standard normal, log density at x=1 is -0.5*log(2π) - 0.5
        log_dens_at_one = self.residuals.LogDensity(1.0)
        expected_log_dens = -0.5 * np.log(2.0 * np.pi) - 0.5
        self.assertAlmostEqual(log_dens_at_one, expected_log_dens, places=10)
    
    def test_diff_log_density(self):
        """Test derivative of log density"""
        # For standard normal, d/dx log f(x) at x=0 is 0
        diff_log_dens_at_zero = self.residuals.DiffLogDensity(0.0)
        self.assertAlmostEqual(diff_log_dens_at_zero, 0.0, places=10)
        
        # For standard normal, d/dx log f(x) at x=1 is -1
        diff_log_dens_at_one = self.residuals.DiffLogDensity(1.0)
        self.assertAlmostEqual(diff_log_dens_at_one, -1.0, places=10)
    
    def test_generate(self):
        """Test random generation"""
        n_samples = 10
        generated = cGSLVector(n_samples)
        self.residuals.Generate(n_samples, generated)
        
        # Check that all values were generated
        for i in range(n_samples):
            # Just check that the values are finite, as they're random
            self.assertFalse(np.isnan(generated[i]))
            self.assertFalse(np.isinf(generated[i]))
    
    def test_compute_esp_abs_eps(self):
        """Test expected absolute value"""
        # For standard normal, E[|X|] = sqrt(2/π)
        esp_abs = self.residuals.ComputeEspAbsEps()
        expected_esp_abs = np.sqrt(2.0 / np.pi)
        self.assertAlmostEqual(esp_abs, expected_esp_abs, places=10)
    
    def test_param_to_vector(self):
        """Test parameter to vector conversion"""
        # Set custom parameters
        self.residuals.mean = 0.5
        self.residuals.std_dev = 2.0
        self.residuals.distr_param[0] = 0.5
        self.residuals.distr_param[1] = 2.0
        
        # Create vector to receive parameters
        params = cGSLVector(2)
        
        # Convert parameters to vector
        self.residuals.RegArchParamToVector(params, 0)
        
        # Check values
        self.assertEqual(params[0], 0.5)
        self.assertEqual(params[1], 2.0)
    
    def test_vector_to_param(self):
        """Test vector to parameter conversion"""
        # Create source vector
        params = cGSLVector(2)
        params[0] = -1.0
        params[1] = 1.5
        
        # Convert vector to parameters
        self.residuals.VectorToRegArchParam(params, 0)
        
        # Check values
        self.assertEqual(self.residuals.mean, -1.0)
        self.assertEqual(self.residuals.std_dev, 1.5)
        self.assertEqual(self.residuals.distr_param[0], -1.0)
        self.assertEqual(self.residuals.distr_param[1], 1.5)
    
    def test_set_default_init_point(self):
        """Test setting default initialization point"""
        # First change parameters from defaults
        self.residuals.mean = 2.0
        self.residuals.std_dev = 3.0
        self.residuals.distr_param[0] = 2.0
        self.residuals.distr_param[1] = 3.0
        
        # Reset to defaults
        self.residuals.SetDefaultInitPoint()
        
        # Check defaults were restored
        self.assertEqual(self.residuals.mean, 0.0)
        self.assertEqual(self.residuals.std_dev, 1.0)
        self.assertEqual(self.residuals.distr_param[0], 0.0)
        self.assertEqual(self.residuals.distr_param[1], 1.0)


if __name__ == "__main__":
    unittest.main()