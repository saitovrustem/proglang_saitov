#include <vector>

extern "C" {
#include <Python.h>
}


namespace floydmodule {
	typedef std::vector<double> row_t;
	typedef std::vector<row_t>  matrix_t;

	static matrix_t floyd_algorithm(matrix_t& a)
	{
		for (size_t k = 0; k < a.size(); ++k) {
			for (size_t i = 0; i < a.size(); ++i) {
				for (size_t j = 0; j < a.size(); ++j) {
					if (a[i][j] != 0 && a[i][k] + a[k][j] != 0) {
						a[i][j] = a[i][j] * (a[i][k] + a[k][j]) / (a[i][j] + a[i][k] + a[k][j]);
					}
				}
			}
		}

		return a;
	}
}

static floydmodule::matrix_t pymatrix_to_cmatrix(PyObject* py_matrix)
{
	floydmodule::matrix_t result;
	result.resize(PyObject_Length(py_matrix));
	for (size_t i = 0; i < result.size(); ++i) {
		PyObject* py_row = PyList_GetItem(py_matrix, i);
		floydmodule::row_t& row = result[i];
		row.resize(PyObject_Length(py_row));
		for (size_t j = 0; j < row.size(); ++j) {
			PyObject* py_elem = PyList_GetItem(py_row, j);
			const double elem = PyFloat_AsDouble(py_elem);
			row[j] = elem;
		}
	}
	return result;
}

static PyObject* cmatrix_to_pymatrix(const floydmodule::matrix_t& matrix)
{
	PyObject* result = PyList_New(matrix.size());
	for (size_t i = 0; i < matrix.size(); ++i) {
		const floydmodule::row_t& row = matrix[i];
		PyObject* py_row = PyList_New(row.size());
		PyList_SetItem(result, i, py_row);
		for (size_t j = 0; j < row.size(); ++j) {
			const double elem = row[j];
			PyObject* py_elem = PyFloat_FromDouble(elem);
			PyList_SetItem(py_row, j, py_elem);
		}
	}
	return result;
}

static PyObject* floyd_algorithm(PyObject* module, PyObject* args)
{
	PyObject* py_a = PyTuple_GetItem(args, 0);
	floydmodule::matrix_t a = pymatrix_to_cmatrix(py_a);
	floydmodule::matrix_t result = floydmodule::floyd_algorithm(a);
	PyObject* py_result = cmatrix_to_pymatrix(result);
	return py_result;
}


PyMODINIT_FUNC PyInit_floydmodule()
{
	static PyMethodDef ModuleMethods[] = {
		{ "floyd_algorithm", floyd_algorithm, METH_VARARGS, "Calculate resistances" },
		{ NULL, NULL, 0, NULL }
	};
	static PyModuleDef ModuleDef = {
		PyModuleDef_HEAD_INIT,
		"floydmodule",
		"Calculate resistances",
		-1, ModuleMethods,
		NULL, NULL, NULL, NULL
	};
	PyObject* module = PyModule_Create(&ModuleDef);
	return module;
}
