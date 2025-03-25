#ifndef PYTHON_CONVERTION_H
#define PYTHON_CONVERTION_H

#include <boost/python.hpp>
#include "StdAfxRegArchLib.h"  // Adjust path if needed

/*!
 * \brief Convert a Python sequence (list or tuple) of floats into a cDVector.
 * \param pyObj A Python object that should be a sequence of floats.
 * \return A newly constructed cDVector containing the float values from that sequence.
 * \throws std::runtime_error if the object is not a list/tuple of floats.
 */
extern RegArchLib::cDVector py_list_or_tuple_to_cDVector(const boost::python::object& pyObj);

/*!
 * \brief Convert a Python list of lists into a cDMatrix.
 * \param pyObj A Python object that should be a list of lists, where each inner list represents a row.
 * \return A newly constructed cDMatrix containing the float values.
 * \throws std::runtime_error if the object is not a list of lists or if rows have inconsistent sizes.
 */
extern RegArchLib::cDMatrix py_list_of_lists_to_cDMatrix(const boost::python::object& pyObj);

#endif // PYTHON_CONVERTION_H
