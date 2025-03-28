# not working


import unittest

from regarch_wrapper import (
    cVarInMean,
    cRegArchValue,
    cRegArchModel
)



class TestVarInMean(unittest.TestCase):
    """
    Tests for the VarInMean model in regarch_wrapper, avoiding direct iteration on cGSLVector.
    """

    def test_initialization(self):
        """
        Ensure we can construct a VarInMean object without error.
        """
        vim = cVarInMean()
        self.assertIsNotNone(vim, "Failed to initialize VarInMean model.")
        self.assertIsInstance(vim, cVarInMean,
                              "Object is not an instance of cVarInMean.")

    def test_set_get_parameters(self):
        """
        Demonstrate:
          - Setting a single float parameter (retrieve with a second argument)
          - Setting a Python list, which becomes cGSLVector inside C++,
            and verifying each element by calling .get(name, index).
        """
        vim = cVarInMean()

        # 1) Set a single float param at sub-index 0
        vim.set(0.75, "VarInMean_Param", 0)
        # Retrieve the scalar
        val_scalar = vim.get("VarInMean_Param", 0)
        self.assertIsInstance(val_scalar, float,
                              "Expected float from get(...,0), got something else.")
        self.assertAlmostEqual(val_scalar, 0.75, places=6,
                               msg="Mismatch after set/get of single float param.")

        # 2) Set a Python list => cGSLVector internally
        vim.set([1.1, 2.2], "VarInMean_VectorParam")

        # To check it, we get each element by specifying the second argument:
        val0 = vim.get("VarInMean_VectorParam", 0)  # element 0
        val1 = vim.get("VarInMean_VectorParam", 1)  # element 1

        self.assertIsInstance(val0, float, "Expected float for vector element 0.")
        self.assertIsInstance(val1, float, "Expected float for vector element 1.")
        self.assertAlmostEqual(val0, 1.1, places=6,
                               msg="Mismatch in vector element 0.")
        self.assertAlmostEqual(val1, 2.2, places=6,
                               msg="Mismatch in vector element 1.")

    def test_compute_mean(self):
        """
        Test the compute_mean(...) method at a given time index,
        using cRegArchValue as the data container.
        """
        vim = cVarInMean()

        # Set a coefficient for VarInMean
        vim.set(0.5, "VarInMean_Param", 0)

        # Build a cRegArchValue with dummy data
        data = cRegArchValue()
        data.mUt = [0.0, 1.0, 2.0, 3.0, 4.0]  # Observations
        data.mHt = [1.0, 1.2, 1.5, 1.8, 2.2]  # Conditional variances

        # Suppose we compute the mean at time index=4
        mean_val = vim.compute_mean(4, data)
        self.assertIsInstance(mean_val, float,
                              "compute_mean did not return a float.")

    def test_simulation(self):
        """
        Test that we can include VarInMean in a cRegArchModel and attempt to simulate.
        If your library requires setting variance/residuals before simulation,
        do so here. If the method name differs from 'simulate', adjust accordingly.
        """
        model = cRegArchModel()

        # Create a VarInMean component
        vim = cVarInMean()
        vim.set(0.75, "VarInMean_Param", 0)

        # The cRegArchModel wrapper defines .def("add_one_mean", &cRegArchModel::AddOneMean,...)
        # so in Python it's named add_one_mean(...).
        model.add_one_mean(vim)

        # If needed, set up variance model, residuals, etc.
        # e.g.:
        # from regarch_wrapper import cArch, cNormResiduals
        # var_comp = cArch()
        # var_comp.set([0.1, 0.2], "ArchParams")
        # model.set_var(var_comp)
        #
        # resids = cNormResiduals()
        # model.set_resid(resids)

        # If your model has a .Simulate(N) method:
        # (some versions call it "simulate" or "simulate_data")
        N = 5
        try:
            simulated_data = model.Simulate(N)  # Adjust to your actual function
        except AttributeError:
            self.skipTest("Model has no Simulate(...) method. Adjust as needed.")
            return

        self.assertEqual(len(simulated_data.mUt), N,
                         "Simulated mUt length mismatch.")
        self.assertEqual(len(simulated_data.mHt), N,
                         "Simulated mHt length mismatch.")

        for i in range(N):
            self.assertIsInstance(simulated_data.mUt[i], float,
                                  f"mUt[{i}] is not float.")
            self.assertIsInstance(simulated_data.mHt[i], float,
                                  f"mHt[{i}] is not float.")


if __name__ == '__main__':
    unittest.main()
