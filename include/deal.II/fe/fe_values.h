// ---------------------------------------------------------------------
//
// Copyright (C) 1998 - 2022 by the deal.II authors
//
// This file is part of the deal.II library.
//
// The deal.II library is free software; you can use it, redistribute
// it, and/or modify it under the terms of the GNU Lesser General
// Public License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
// The full text of the license can be found in the file LICENSE.md at
// the top level directory of deal.II.
//
// ---------------------------------------------------------------------

#ifndef dealii_fe_values_h
#define dealii_fe_values_h


#include <deal.II/base/config.h>

#include <deal.II/base/derivative_form.h>
#include <deal.II/base/exceptions.h>
#include <deal.II/base/point.h>
#include <deal.II/base/quadrature.h>
#include <deal.II/base/std_cxx20/iota_view.h>
#include <deal.II/base/subscriptor.h>
#include <deal.II/base/symmetric_tensor.h>

#include <deal.II/dofs/dof_accessor.h>
#include <deal.II/dofs/dof_handler.h>

#include <deal.II/fe/fe.h>
#include <deal.II/fe/fe_update_flags.h>
#include <deal.II/fe/fe_values_extractors.h>
#include <deal.II/fe/mapping.h>
#include <deal.II/fe/mapping_related_data.h>

#include <deal.II/grid/tria.h>
#include <deal.II/grid/tria_iterator.h>

#include <deal.II/hp/q_collection.h>

#include <algorithm>
#include <memory>
#include <type_traits>


// dummy include in order to have the
// definition of PetscScalar available
// without including other PETSc stuff
#ifdef DEAL_II_WITH_PETSC
#  include <petsc.h>
#endif

DEAL_II_NAMESPACE_OPEN

// Forward declaration
#ifndef DOXYGEN
template <int dim, int spacedim = dim>
class FEValuesBase;
#endif

namespace internal
{
  /**
   * A class whose specialization is used to define what type the curl of a
   * vector valued function corresponds to.
   */
  template <int dim, class NumberType = double>
  struct CurlType;

  /**
   * A class whose specialization is used to define what type the curl of a
   * vector valued function corresponds to.
   *
   * In 1d, the curl is a scalar.
   */
  template <class NumberType>
  struct CurlType<1, NumberType>
  {
    using type = Tensor<1, 1, NumberType>;
  };

  /**
   * A class whose specialization is used to define what type the curl of a
   * vector valued function corresponds to.
   *
   * In 2d, the curl is a scalar.
   */
  template <class NumberType>
  struct CurlType<2, NumberType>
  {
    using type = Tensor<1, 1, NumberType>;
  };

  /**
   * A class whose specialization is used to define what type the curl of a
   * vector valued function corresponds to.
   *
   * In 3d, the curl is a vector.
   */
  template <class NumberType>
  struct CurlType<3, NumberType>
  {
    using type = Tensor<1, 3, NumberType>;
  };
} // namespace internal



/**
 * A namespace for "views" on a FEValues, FEFaceValues, or FESubfaceValues
 * object. A view represents only a certain part of the whole: whereas the
 * FEValues object represents <i>all</i> values, gradients, or second
 * derivatives of all components of a vector-valued element, views restrict
 * the attention to only a single component or a subset of components. You
 * typically get objects of classes defined in this namespace by applying
 * FEValuesExtractors objects to a FEValues, FEFaceValues or FESubfaceValues
 * objects using the square bracket operator.
 *
 * There are classes that present views for single scalar components, vector
 * components consisting of <code>dim</code> elements, and symmetric second
 * order tensor components consisting of <code>(dim*dim + dim)/2</code>
 * elements
 *
 * See the description of the
 * @ref vector_valued
 * module for examples how to use the features of this namespace.
 *
 * @ingroup feaccess vector_valued
 */
namespace FEValuesViews
{
  /**
   * A class representing a view to a single scalar component of a possibly
   * vector-valued finite element. Views are discussed in the
   * @ref vector_valued
   * module.
   *
   * You get an object of this type if you apply a FEValuesExtractors::Scalar
   * to an FEValues, FEFaceValues or FESubfaceValues object.
   *
   * @ingroup feaccess vector_valued
   */
  template <int dim, int spacedim = dim>
  class Scalar
  {
  public:
    /**
     * An alias for the data type of values of the view this class
     * represents. Since we deal with a single components, the value type is a
     * scalar double.
     */
    using value_type = double;

    /**
     * An alias for the type of gradients of the view this class represents.
     * Here, for a scalar component of the finite element, the gradient is a
     * <code>Tensor@<1,dim@></code>.
     */
    using gradient_type = dealii::Tensor<1, spacedim>;

    /**
     * An alias for the type of second derivatives of the view this class
     * represents. Here, for a scalar component of the finite element, the
     * Hessian is a <code>Tensor@<2,dim@></code>.
     */
    using hessian_type = dealii::Tensor<2, spacedim>;

    /**
     * An alias for the type of third derivatives of the view this class
     * represents. Here, for a scalar component of the finite element, the
     * Third derivative is a <code>Tensor@<3,dim@></code>.
     */
    using third_derivative_type = dealii::Tensor<3, spacedim>;

    /**
     * An alias for the data type of the product of a @p Number and the
     * values of the view this class provides. This is the data type of
     * scalar components of a finite element field whose degrees of
     * freedom are described by a vector with elements of type @p Number.
     */
    template <typename Number>
    using solution_value_type = typename ProductType<Number, value_type>::type;

    /**
     * An alias for the data type of the product of a @p Number and the
     * gradients of the view this class provides. This is the data type of
     * scalar components of a finite element field whose degrees of
     * freedom are described by a vector with elements of type @p Number.
     */
    template <typename Number>
    using solution_gradient_type =
      typename ProductType<Number, gradient_type>::type;

    /**
     * An alias for the data type of the product of a @p Number and the
     * laplacians of the view this class provides. This is the data type of
     * scalar components of a finite element field whose degrees of
     * freedom are described by a vector with elements of type @p Number.
     */
    template <typename Number>
    using solution_laplacian_type =
      typename ProductType<Number, value_type>::type;

    /**
     * An alias for the data type of the product of a @p Number and the
     * hessians of the view this class provides. This is the data type of
     * scalar components of a finite element field whose degrees of
     * freedom are described by a vector with elements of type @p Number.
     */
    template <typename Number>
    using solution_hessian_type =
      typename ProductType<Number, hessian_type>::type;

    /**
     * An alias for the data type of the product of a @p Number and the
     * third derivatives of the view this class provides. This is the data type
     * of scalar components of a finite element field whose degrees of
     * freedom are described by a vector with elements of type @p Number.
     */
    template <typename Number>
    using solution_third_derivative_type =
      typename ProductType<Number, third_derivative_type>::type;

    /**
     * A struct that provides the output type for the product of the value
     * and derivatives of basis functions of the Scalar view and any @p Number type.
     *
     * @deprecated Use the types defined in the surrounding class instead.
     */
    template <typename Number>
    struct DEAL_II_DEPRECATED OutputType
    {
      /**
       * An alias for the data type of the product of a @p Number and the
       * values of the view the Scalar class.
       */
      using value_type =
        typename ProductType<Number,
                             typename Scalar<dim, spacedim>::value_type>::type;

      /**
       * An alias for the data type of the product of a @p Number and the
       * gradients of the view the Scalar class.
       */
      using gradient_type = typename ProductType<
        Number,
        typename Scalar<dim, spacedim>::gradient_type>::type;

      /**
       * An alias for the data type of the product of a @p Number and the
       * laplacians of the view the Scalar class.
       */
      using laplacian_type =
        typename ProductType<Number,
                             typename Scalar<dim, spacedim>::value_type>::type;

      /**
       * An alias for the data type of the product of a @p Number and the
       * hessians of the view the Scalar class.
       */
      using hessian_type = typename ProductType<
        Number,
        typename Scalar<dim, spacedim>::hessian_type>::type;

      /**
       * An alias for the data type of the product of a @p Number and the
       * third derivatives of the view the Scalar class.
       */
      using third_derivative_type = typename ProductType<
        Number,
        typename Scalar<dim, spacedim>::third_derivative_type>::type;
    };

    /**
     * A structure where for each shape function we pre-compute a bunch of
     * data that will make later accesses much cheaper.
     */
    struct ShapeFunctionData
    {
      /**
       * For each shape function, store whether the selected vector component
       * may be nonzero. For primitive shape functions we know for sure
       * whether a certain scalar component of a given shape function is
       * nonzero, whereas for non-primitive shape functions this may not be
       * entirely clear (e.g. for RT elements it depends on the shape of a
       * cell).
       */
      bool is_nonzero_shape_function_component;

      /**
       * For each shape function, store the row index within the shape_values,
       * shape_gradients, and shape_hessians tables (the column index is the
       * quadrature point index). If the shape function is primitive, then we
       * can get this information from the shape_function_to_row_table of the
       * FEValues object; otherwise, we have to work a bit harder to compute
       * this information.
       */
      unsigned int row_index;
    };

    /**
     * Default constructor. Creates an invalid object.
     */
    Scalar();

    /**
     * Constructor for an object that represents a single scalar component of
     * a FEValuesBase object (or of one of the classes derived from
     * FEValuesBase).
     */
    Scalar(const FEValuesBase<dim, spacedim> &fe_values_base,
           const unsigned int                 component);

    /**
     * Copy constructor. This is not a lightweight object so we don't allow
     * copying and generate a compile-time error if this function is called.
     */
    Scalar(const Scalar<dim, spacedim> &) = delete;

    /**
     * Move constructor.
     */
    // NOLINTNEXTLINE OSX does not compile with noexcept
    Scalar(Scalar<dim, spacedim> &&) = default;

    /**
     * Destructor.
     */
    ~Scalar() = default;

    /**
     * Copy operator. This is not a lightweight object so we don't allow
     * copying and generate a compile-time error if this function is called.
     */
    Scalar &
    operator=(const Scalar<dim, spacedim> &) = delete;

    /**
     * Move assignment operator.
     */
    Scalar &
    operator=(Scalar<dim, spacedim> &&) noexcept = default;

    /**
     * Return the value of the vector component selected by this view, for the
     * shape function and quadrature point selected by the arguments.
     *
     * @param shape_function Number of the shape function to be evaluated.
     * Note that this number runs from zero to dofs_per_cell, even in the case
     * of an FEFaceValues or FESubfaceValues object.
     *
     * @param q_point Number of the quadrature point at which function is to
     * be evaluated.
     *
     * @dealiiRequiresUpdateFlags{update_values}
     */
    value_type
    value(const unsigned int shape_function, const unsigned int q_point) const;

    /**
     * Return the gradient (a tensor of rank 1) of the vector component
     * selected by this view, for the shape function and quadrature point
     * selected by the arguments.
     *
     * @note The meaning of the arguments is as documented for the value()
     * function.
     *
     * @dealiiRequiresUpdateFlags{update_gradients}
     */
    gradient_type
    gradient(const unsigned int shape_function,
             const unsigned int q_point) const;

    /**
     * Return the Hessian (the tensor of rank 2 of all second derivatives) of
     * the vector component selected by this view, for the shape function and
     * quadrature point selected by the arguments.
     *
     * @note The meaning of the arguments is as documented for the value()
     * function.
     *
     * @dealiiRequiresUpdateFlags{update_hessians}
     */
    hessian_type
    hessian(const unsigned int shape_function,
            const unsigned int q_point) const;

    /**
     * Return the tensor of rank 3 of all third derivatives of the vector
     * component selected by this view, for the shape function and quadrature
     * point selected by the arguments.
     *
     * @note The meaning of the arguments is as documented for the value()
     * function.
     *
     * @dealiiRequiresUpdateFlags{update_third_derivatives}
     */
    third_derivative_type
    third_derivative(const unsigned int shape_function,
                     const unsigned int q_point) const;

    /**
     * Return the values of the selected scalar component of the finite
     * element function characterized by <tt>fe_function</tt> at the
     * quadrature points of the cell, face or subface selected the last time
     * the <tt>reinit</tt> function of the FEValues object was called.
     *
     * This function is the equivalent of the
     * FEValuesBase::get_function_values function but it only works on the
     * selected scalar component.
     *
     * The data type stored by the output vector must be what you get when you
     * multiply the values of shape functions (i.e., @p value_type) times the
     * type used to store the values of the unknowns $U_j$ of your finite
     * element vector $U$ (represented by the @p fe_function argument).
     *
     * @dealiiRequiresUpdateFlags{update_values}
     */
    template <class InputVector>
    void
    get_function_values(
      const InputVector &fe_function,
      std::vector<solution_value_type<typename InputVector::value_type>>
        &values) const;

    /**
     * Same as above, but using a vector of local degree-of-freedom values. In
     * other words, instead of extracting the nodal values of the degrees of
     * freedom located on the current cell from a global vector associated with
     * a DoFHandler object (as the function above does), this function instead
     * takes these local nodal values through its first argument. A typical
     * way to obtain such a vector is by calling code such as
     * @code
     *   cell->get_dof_values (dof_values, local_dof_values);
     * @endcode
     * (See DoFCellAccessor::get_dof_values() for more information on this
     * function.) The point of the current function is then that one could
     * modify these local values first, for example by applying a limiter
     * or by ensuring that all nodal values are positive, before evaluating
     * the finite element field that corresponds to these local values on the
     * current cell. Another application is where one wants to postprocess
     * the solution on a cell into a different finite element space on every
     * cell, without actually creating a corresponding DoFHandler -- in that
     * case, all one would compute is a local representation of that
     * postprocessed function, characterized by its nodal values; this function
     * then allows the evaluation of that representation at quadrature points.
     *
     * @param[in] dof_values A vector of local nodal values. This vector must
     *   have a length equal to number of DoFs on the current cell, and must
     *   be ordered in the same order as degrees of freedom are numbered on
     *   the reference cell.
     *
     * @param[out] values A vector of values of the given finite element field,
     *   at the quadrature points on the current object.
     *
     * @tparam InputVector The @p InputVector type must allow creation
     *   of an ArrayView object from it; this is satisfied by the
     *   `std::vector` class, among others.
     */
    template <class InputVector>
    void
    get_function_values_from_local_dof_values(
      const InputVector &dof_values,
      std::vector<solution_value_type<typename InputVector::value_type>>
        &values) const;

    /**
     * Return the gradients of the selected scalar component of the finite
     * element function characterized by <tt>fe_function</tt> at the
     * quadrature points of the cell, face or subface selected the last time
     * the <tt>reinit</tt> function of the FEValues object was called.
     *
     * This function is the equivalent of the
     * FEValuesBase::get_function_gradients function but it only works on the
     * selected scalar component.
     *
     * The data type stored by the output vector must be what you get when you
     * multiply the gradients of shape functions (i.e., @p gradient_type)
     * times the type used to store the values of the unknowns $U_j$ of your
     * finite element vector $U$ (represented by the @p fe_function argument).
     *
     * @dealiiRequiresUpdateFlags{update_gradients}
     */
    template <class InputVector>
    void
    get_function_gradients(
      const InputVector &fe_function,
      std::vector<solution_gradient_type<typename InputVector::value_type>>
        &gradients) const;

    /**
     * This function relates to get_function_gradients() in the same way
     * as get_function_values_from_local_dof_values() relates to
     * get_function_values(). See the documentation of
     * get_function_values_from_local_dof_values() for more information.
     */
    template <class InputVector>
    void
    get_function_gradients_from_local_dof_values(
      const InputVector &dof_values,
      std::vector<solution_gradient_type<typename InputVector::value_type>>
        &gradients) const;

    /**
     * Return the Hessians of the selected scalar component of the finite
     * element function characterized by <tt>fe_function</tt> at the
     * quadrature points of the cell, face or subface selected the last time
     * the <tt>reinit</tt> function of the FEValues object was called.
     *
     * This function is the equivalent of the
     * FEValuesBase::get_function_hessians function but it only works on the
     * selected scalar component.
     *
     * The data type stored by the output vector must be what you get when you
     * multiply the Hessians of shape functions (i.e., @p hessian_type) times
     * the type used to store the values of the unknowns $U_j$ of your finite
     * element vector $U$ (represented by the @p fe_function argument).
     *
     * @dealiiRequiresUpdateFlags{update_hessians}
     */
    template <class InputVector>
    void
    get_function_hessians(
      const InputVector &fe_function,
      std::vector<solution_hessian_type<typename InputVector::value_type>>
        &hessians) const;

    /**
     * This function relates to get_function_hessians() in the same way
     * as get_function_values_from_local_dof_values() relates to
     * get_function_values(). See the documentation of
     * get_function_values_from_local_dof_values() for more information.
     */
    template <class InputVector>
    void
    get_function_hessians_from_local_dof_values(
      const InputVector &dof_values,
      std::vector<solution_hessian_type<typename InputVector::value_type>>
        &hessians) const;


    /**
     * Return the Laplacians of the selected scalar component of the finite
     * element function characterized by <tt>fe_function</tt> at the
     * quadrature points of the cell, face or subface selected the last time
     * the <tt>reinit</tt> function of the FEValues object was called. The
     * Laplacians are the trace of the Hessians.
     *
     * This function is the equivalent of the
     * FEValuesBase::get_function_laplacians function but it only works on the
     * selected scalar component.
     *
     * The data type stored by the output vector must be what you get when you
     * multiply the Laplacians of shape functions (i.e., @p value_type) times
     * the type used to store the values of the unknowns $U_j$ of your finite
     * element vector $U$ (represented by the @p fe_function argument).
     *
     * @dealiiRequiresUpdateFlags{update_hessians}
     */
    template <class InputVector>
    void
    get_function_laplacians(
      const InputVector &fe_function,
      std::vector<solution_laplacian_type<typename InputVector::value_type>>
        &laplacians) const;

    /**
     * This function relates to get_function_laplacians() in the same way
     * as get_function_values_from_local_dof_values() relates to
     * get_function_values(). See the documentation of
     * get_function_values_from_local_dof_values() for more information.
     */
    template <class InputVector>
    void
    get_function_laplacians_from_local_dof_values(
      const InputVector &dof_values,
      std::vector<solution_laplacian_type<typename InputVector::value_type>>
        &laplacians) const;


    /**
     * Return the third derivatives of the selected scalar component of the
     * finite element function characterized by <tt>fe_function</tt> at the
     * quadrature points of the cell, face or subface selected the last time
     * the <tt>reinit</tt> function of the FEValues object was called.
     *
     * This function is the equivalent of the
     * FEValuesBase::get_function_third_derivatives function but it only works
     * on the selected scalar component.
     *
     * The data type stored by the output vector must be what you get when you
     * multiply the third derivatives of shape functions (i.e., @p
     * third_derivative_type) times the type used to store the values of the
     * unknowns $U_j$ of your finite element vector $U$ (represented by the @p
     * fe_function argument).
     *
     * @dealiiRequiresUpdateFlags{update_third_derivatives}
     */
    template <class InputVector>
    void
    get_function_third_derivatives(
      const InputVector &fe_function,
      std::vector<
        solution_third_derivative_type<typename InputVector::value_type>>
        &third_derivatives) const;

    /**
     * This function relates to get_function_third_derivatives() in the same way
     * as get_function_values_from_local_dof_values() relates to
     * get_function_values(). See the documentation of
     * get_function_values_from_local_dof_values() for more information.
     */
    template <class InputVector>
    void
    get_function_third_derivatives_from_local_dof_values(
      const InputVector &dof_values,
      std::vector<
        solution_third_derivative_type<typename InputVector::value_type>>
        &third_derivatives) const;


  private:
    /**
     * A pointer to the FEValuesBase object we operate on.
     */
    const SmartPointer<const FEValuesBase<dim, spacedim>> fe_values;

    /**
     * The single scalar component this view represents of the FEValuesBase
     * object.
     */
    const unsigned int component;

    /**
     * Store the data about shape functions.
     */
    std::vector<ShapeFunctionData> shape_function_data;
  };



  /**
   * A class representing a view to a set of <code>spacedim</code> components
   * forming a vector part of a vector-valued finite element. Views are
   * discussed in the
   * @ref vector_valued
   * module.
   *
   * Note that in the current context, a vector is meant in the sense physics
   * uses it: it has <code>spacedim</code> components that behave in specific
   * ways under coordinate system transformations. Examples include velocity
   * or displacement fields. This is opposed to how mathematics uses the word
   * "vector" (and how we use this word in other contexts in the library, for
   * example in the Vector class), where it really stands for a collection of
   * numbers. An example of this latter use of the word could be the set of
   * concentrations of chemical species in a flame; however, these are really
   * just a collection of scalar variables, since they do not change if the
   * coordinate system is rotated, unlike the components of a velocity vector,
   * and consequently, this class should not be used for this context.
   *
   * This class allows to query the value, gradient and divergence of
   * (components of) shape functions and solutions representing vectors. The
   * gradient of a vector $d_{k}, 0\le k<\text{dim}$ is defined as $S_{ij} =
   * \frac{\partial d_{i}}{\partial x_j}, 0\le i,j<\text{dim}$.
   *
   * You get an object of this type if you apply a FEValuesExtractors::Vector
   * to an FEValues, FEFaceValues or FESubfaceValues object.
   *
   * @ingroup feaccess vector_valued
   */
  template <int dim, int spacedim = dim>
  class Vector
  {
  public:
    /**
     * An alias for the data type of values of the view this class
     * represents. Since we deal with a set of <code>dim</code> components,
     * the value type is a Tensor<1,spacedim>.
     */
    using value_type = dealii::Tensor<1, spacedim>;

    /**
     * An alias for the type of gradients of the view this class represents.
     * Here, for a set of <code>dim</code> components of the finite element,
     * the gradient is a <code>Tensor@<2,spacedim@></code>.
     *
     * See the general documentation of this class for how exactly the
     * gradient of a vector is defined.
     */
    using gradient_type = dealii::Tensor<2, spacedim>;

    /**
     * An alias for the type of symmetrized gradients of the view this class
     * represents. Here, for a set of <code>dim</code> components of the
     * finite element, the symmetrized gradient is a
     * <code>SymmetricTensor@<2,spacedim@></code>.
     *
     * The symmetric gradient of a vector field $\mathbf v$ is defined as
     * $\varepsilon(\mathbf v)=\frac 12 (\nabla \mathbf v + \nabla \mathbf
     * v^T)$.
     */
    using symmetric_gradient_type = dealii::SymmetricTensor<2, spacedim>;

    /**
     * An alias for the type of the divergence of the view this class
     * represents. Here, for a set of <code>dim</code> components of the
     * finite element, the divergence of course is a scalar.
     */
    using divergence_type = double;

    /**
     * An alias for the type of the curl of the view this class represents.
     * Here, for a set of <code>spacedim=2</code> components of the finite
     * element, the curl is a <code>Tensor@<1, 1@></code>. For
     * <code>spacedim=3</code> it is a <code>Tensor@<1, dim@></code>.
     */
    using curl_type = typename dealii::internal::CurlType<spacedim>::type;

    /**
     * An alias for the type of second derivatives of the view this class
     * represents. Here, for a set of <code>dim</code> components of the
     * finite element, the Hessian is a <code>Tensor@<3,dim@></code>.
     */
    using hessian_type = dealii::Tensor<3, spacedim>;

    /**
     * An alias for the type of third derivatives of the view this class
     * represents. Here, for a set of <code>dim</code> components of the
     * finite element, the third derivative is a <code>Tensor@<4,dim@></code>.
     */
    using third_derivative_type = dealii::Tensor<4, spacedim>;

    /**
     * An alias for the data type of the product of a @p Number and the
     * values of the view this class provides. This is the data type of
     * vector components of a finite element field whose degrees of
     * freedom are described by a vector with elements of type @p Number.
     */
    template <typename Number>
    using solution_value_type = typename ProductType<Number, value_type>::type;

    /**
     * An alias for the data type of the product of a @p Number and the
     * gradients of the view this class provides. This is the data type of
     * vector components of a finite element field whose degrees of
     * freedom are described by a vector with elements of type @p Number.
     */
    template <typename Number>
    using solution_gradient_type =
      typename ProductType<Number, gradient_type>::type;

    /**
     * An alias for the data type of the product of a @p Number and the
     * symmetric gradients of the view this class provides. This is the data
     * type of vector components of a finite element field whose degrees of
     * freedom are described by a vector with elements of type @p Number.
     */
    template <typename Number>
    using solution_symmetric_gradient_type =
      typename ProductType<Number, symmetric_gradient_type>::type;

    /**
     * An alias for the data type of the product of a @p Number and the
     * divergences of the view this class provides. This is the data type of
     * vector components of a finite element field whose degrees of
     * freedom are described by a vector with elements of type @p Number.
     */
    template <typename Number>
    using solution_divergence_type =
      typename ProductType<Number, divergence_type>::type;

    /**
     * An alias for the data type of the product of a @p Number and the
     * laplacians of the view this class provides. This is the data type of
     * vector components of a finite element field whose degrees of
     * freedom are described by a vector with elements of type @p Number.
     */
    template <typename Number>
    using solution_laplacian_type =
      typename ProductType<Number, value_type>::type;

    /**
     * An alias for the data type of the product of a @p Number and the
     * curls of the view this class provides. This is the data type of
     * vector components of a finite element field whose degrees of
     * freedom are described by a vector with elements of type @p Number.
     */
    template <typename Number>
    using solution_curl_type = typename ProductType<Number, curl_type>::type;

    /**
     * An alias for the data type of the product of a @p Number and the
     * hessians of the view this class provides. This is the data type of
     * vector components of a finite element field whose degrees of
     * freedom are described by a vector with elements of type @p Number.
     */
    template <typename Number>
    using solution_hessian_type =
      typename ProductType<Number, hessian_type>::type;

    /**
     * An alias for the data type of the product of a @p Number and the
     * third derivatives of the view this class provides. This is the data type
     * of vector components of a finite element field whose degrees of
     * freedom are described by a vector with elements of type @p Number.
     */
    template <typename Number>
    using solution_third_derivative_type =
      typename ProductType<Number, third_derivative_type>::type;

    /**
     * A struct that provides the output type for the product of the value
     * and derivatives of basis functions of the Vector view and any @p Number type.
     *
     * @deprecated Use the types defined in the surrounding class instead.
     */
    template <typename Number>
    struct DEAL_II_DEPRECATED OutputType
    {
      /**
       * An alias for the data type of the product of a @p Number and the
       * values of the view the Vector class.
       */
      using value_type =
        typename ProductType<Number,
                             typename Vector<dim, spacedim>::value_type>::type;

      /**
       * An alias for the data type of the product of a @p Number and the
       * gradients of the view the Vector class.
       */
      using gradient_type = typename ProductType<
        Number,
        typename Vector<dim, spacedim>::gradient_type>::type;

      /**
       * An alias for the data type of the product of a @p Number and the
       * symmetric gradients of the view the Vector class.
       */
      using symmetric_gradient_type = typename ProductType<
        Number,
        typename Vector<dim, spacedim>::symmetric_gradient_type>::type;

      /**
       * An alias for the data type of the product of a @p Number and the
       * divergences of the view the Vector class.
       */
      using divergence_type = typename ProductType<
        Number,
        typename Vector<dim, spacedim>::divergence_type>::type;

      /**
       * An alias for the data type of the product of a @p Number and the
       * laplacians of the view the Vector class.
       */
      using laplacian_type =
        typename ProductType<Number,
                             typename Vector<dim, spacedim>::value_type>::type;

      /**
       * An alias for the data type of the product of a @p Number and the
       * curls of the view the Vector class.
       */
      using curl_type =
        typename ProductType<Number,
                             typename Vector<dim, spacedim>::curl_type>::type;

      /**
       * An alias for the data type of the product of a @p Number and the
       * hessians of the view the Vector class.
       */
      using hessian_type = typename ProductType<
        Number,
        typename Vector<dim, spacedim>::hessian_type>::type;

      /**
       * An alias for the data type of the product of a @p Number and the
       * third derivatives of the view the Vector class.
       */
      using third_derivative_type = typename ProductType<
        Number,
        typename Vector<dim, spacedim>::third_derivative_type>::type;
    };

    /**
     * A structure where for each shape function we pre-compute a bunch of
     * data that will make later accesses much cheaper.
     */
    struct ShapeFunctionData
    {
      /**
       * For each pair (shape function,component within vector), store whether
       * the selected vector component may be nonzero. For primitive shape
       * functions we know for sure whether a certain scalar component of a
       * given shape function is nonzero, whereas for non-primitive shape
       * functions this may not be entirely clear (e.g. for RT elements it
       * depends on the shape of a cell).
       */
      bool is_nonzero_shape_function_component[spacedim];

      /**
       * For each pair (shape function, component within vector), store the
       * row index within the shape_values, shape_gradients, and
       * shape_hessians tables (the column index is the quadrature point
       * index). If the shape function is primitive, then we can get this
       * information from the shape_function_to_row_table of the FEValues
       * object; otherwise, we have to work a bit harder to compute this
       * information.
       */
      unsigned int row_index[spacedim];

      /**
       * For each shape function say the following: if only a single entry in
       * is_nonzero_shape_function_component for this shape function is
       * nonzero, then store the corresponding value of row_index and
       * single_nonzero_component_index represents the index between 0 and dim
       * for which it is attained. If multiple components are nonzero, then
       * store -1. If no components are nonzero then store -2.
       */
      int          single_nonzero_component;
      unsigned int single_nonzero_component_index;
    };

    /**
     * Default constructor. Creates an invalid object.
     */
    Vector();

    /**
     * Constructor for an object that represents dim components of a
     * FEValuesBase object (or of one of the classes derived from
     * FEValuesBase), representing a vector-valued variable.
     *
     * The second argument denotes the index of the first component of the
     * selected vector.
     */
    Vector(const FEValuesBase<dim, spacedim> &fe_values_base,
           const unsigned int                 first_vector_component);

    /**
     * Copy constructor. This is not a lightweight object so we don't allow
     * copying and generate a compile-time error if this function is called.
     */
    Vector(const Vector<dim, spacedim> &) = delete;

    /**
     * Move constructor.
     */
    // NOLINTNEXTLINE OSX does not compile with noexcept
    Vector(Vector<dim, spacedim> &&) = default;

    /**
     * Destructor.
     */
    ~Vector() = default;

    /**
     * Copy operator. This is not a lightweight object so we don't allow
     * copying and generate a compile-time error if this function is called.
     */
    Vector &
    operator=(const Vector<dim, spacedim> &) = delete;

    /**
     * Move assignment operator.
     */
    // NOLINTNEXTLINE OSX does not compile with noexcept
    Vector &
    operator=(Vector<dim, spacedim> &&) = default; // NOLINT

    /**
     * Return the value of the vector components selected by this view, for
     * the shape function and quadrature point selected by the arguments.
     * Here, since the view represents a vector-valued part of the FEValues
     * object with <code>dim</code> components, the return type is a tensor of
     * rank 1 with <code>dim</code> components.
     *
     * @param shape_function Number of the shape function to be evaluated.
     * Note that this number runs from zero to dofs_per_cell, even in the case
     * of an FEFaceValues or FESubfaceValues object.
     *
     * @param q_point Number of the quadrature point at which function is to
     * be evaluated.
     *
     * @dealiiRequiresUpdateFlags{update_values}
     */
    value_type
    value(const unsigned int shape_function, const unsigned int q_point) const;

    /**
     * Return the gradient (a tensor of rank 2) of the vector component
     * selected by this view, for the shape function and quadrature point
     * selected by the arguments.
     *
     * See the general documentation of this class for how exactly the
     * gradient of a vector is defined.
     *
     * @note The meaning of the arguments is as documented for the value()
     * function.
     *
     * @dealiiRequiresUpdateFlags{update_gradients}
     */
    gradient_type
    gradient(const unsigned int shape_function,
             const unsigned int q_point) const;

    /**
     * Return the symmetric gradient (a symmetric tensor of rank 2) of the
     * vector component selected by this view, for the shape function and
     * quadrature point selected by the arguments.
     *
     * The symmetric gradient is defined as $\frac 12 [(\nabla \phi_i(x_q)) +
     * (\nabla \phi_i(x_q))^T]$, where $\phi_i$ represents the
     * <code>dim</code> components selected from the FEValuesBase object, and
     * $x_q$ is the location of the $q$-th quadrature point.
     *
     * @note The meaning of the arguments is as documented for the value()
     * function.
     *
     * @dealiiRequiresUpdateFlags{update_gradients}
     */
    symmetric_gradient_type
    symmetric_gradient(const unsigned int shape_function,
                       const unsigned int q_point) const;

    /**
     * Return the scalar divergence of the vector components selected by this
     * view, for the shape function and quadrature point selected by the
     * arguments.
     *
     * @note The meaning of the arguments is as documented for the value()
     * function.
     *
     * @dealiiRequiresUpdateFlags{update_gradients}
     */
    divergence_type
    divergence(const unsigned int shape_function,
               const unsigned int q_point) const;

    /**
     * Return the vector curl of the vector components selected by this view,
     * for the shape function and quadrature point selected by the arguments.
     * For 1d this function does not make any sense. Thus it is not
     * implemented for <code>spacedim=1</code>.  In 2d the curl is defined as
     * @f{equation*}{
     * \operatorname{curl}(u) \dealcoloneq \frac{du_2}{dx} -\frac{du_1}{dy},
     * @f}
     * whereas in 3d it is given by
     * @f{equation*}{
     * \operatorname{curl}(u) \dealcoloneq \left( \begin{array}{c}
     * \frac{du_3}{dy}-\frac{du_2}{dz}\\ \frac{du_1}{dz}-\frac{du_3}{dx}\\
     * \frac{du_2}{dx}-\frac{du_1}{dy} \end{array} \right).
     * @f}
     *
     * @note The meaning of the arguments is as documented for the value()
     * function.
     *
     * @dealiiRequiresUpdateFlags{update_gradients}
     */
    curl_type
    curl(const unsigned int shape_function, const unsigned int q_point) const;

    /**
     * Return the Hessian (the tensor of rank 2 of all second derivatives) of
     * the vector components selected by this view, for the shape function and
     * quadrature point selected by the arguments.
     *
     * @note The meaning of the arguments is as documented for the value()
     * function.
     *
     * @dealiiRequiresUpdateFlags{update_hessians}
     */
    hessian_type
    hessian(const unsigned int shape_function,
            const unsigned int q_point) const;

    /**
     * Return the tensor of rank 3 of all third derivatives of the vector
     * components selected by this view, for the shape function and quadrature
     * point selected by the arguments.
     *
     * @note The meaning of the arguments is as documented for the value()
     * function.
     *
     * @dealiiRequiresUpdateFlags{update_3rd_derivatives}
     */
    third_derivative_type
    third_derivative(const unsigned int shape_function,
                     const unsigned int q_point) const;

    /**
     * Return the values of the selected vector components of the finite
     * element function characterized by <tt>fe_function</tt> at the
     * quadrature points of the cell, face or subface selected the last time
     * the <tt>reinit</tt> function of the FEValues object was called.
     *
     * This function is the equivalent of the
     * FEValuesBase::get_function_values function but it only works on the
     * selected vector components.
     *
     * The data type stored by the output vector must be what you get when you
     * multiply the values of shape functions (i.e., @p value_type) times the
     * type used to store the values of the unknowns $U_j$ of your finite
     * element vector $U$ (represented by the @p fe_function argument).
     *
     * @dealiiRequiresUpdateFlags{update_values}
     */
    template <class InputVector>
    void
    get_function_values(
      const InputVector &fe_function,
      std::vector<solution_value_type<typename InputVector::value_type>>
        &values) const;

    /**
     * Same as above, but using a vector of local degree-of-freedom values. In
     * other words, instead of extracting the nodal values of the degrees of
     * freedom located on the current cell from a global vector associated with
     * a DoFHandler object (as the function above does), this function instead
     * takes these local nodal values through its first argument. A typical
     * way to obtain such a vector is by calling code such as
     * @code
     *   cell->get_dof_values (dof_values, local_dof_values);
     * @endcode
     * (See DoFCellAccessor::get_dof_values() for more information on this
     * function.) The point of the current function is then that one could
     * modify these local values first, for example by applying a limiter
     * or by ensuring that all nodal values are positive, before evaluating
     * the finite element field that corresponds to these local values on the
     * current cell. Another application is where one wants to postprocess
     * the solution on a cell into a different finite element space on every
     * cell, without actually creating a corresponding DoFHandler -- in that
     * case, all one would compute is a local representation of that
     * postprocessed function, characterized by its nodal values; this function
     * then allows the evaluation of that representation at quadrature points.
     *
     * @param[in] dof_values A vector of local nodal values. This vector must
     *   have a length equal to number of DoFs on the current cell, and must
     *   be ordered in the same order as degrees of freedom are numbered on
     *   the reference cell.
     *
     * @param[out] values A vector of values of the given finite element field,
     *   at the quadrature points on the current object.
     *
     * @tparam InputVector The @p InputVector type must allow creation
     *   of an ArrayView object from it; this is satisfied by the
     *   `std::vector` class, among others.
     */
    template <class InputVector>
    void
    get_function_values_from_local_dof_values(
      const InputVector &dof_values,
      std::vector<solution_value_type<typename InputVector::value_type>>
        &values) const;

    /**
     * Return the gradients of the selected vector components of the finite
     * element function characterized by <tt>fe_function</tt> at the
     * quadrature points of the cell, face or subface selected the last time
     * the <tt>reinit</tt> function of the FEValues object was called.
     *
     * This function is the equivalent of the
     * FEValuesBase::get_function_gradients function but it only works on the
     * selected vector components.
     *
     * The data type stored by the output vector must be what you get when you
     * multiply the gradients of shape functions (i.e., @p gradient_type)
     * times the type used to store the values of the unknowns $U_j$ of your
     * finite element vector $U$ (represented by the @p fe_function argument).
     *
     * @dealiiRequiresUpdateFlags{update_gradients}
     */
    template <class InputVector>
    void
    get_function_gradients(
      const InputVector &fe_function,
      std::vector<solution_gradient_type<typename InputVector::value_type>>
        &gradients) const;

    /**
     * This function relates to get_function_gradients() in the same way
     * as get_function_values_from_local_dof_values() relates to
     * get_function_values(). See the documentation of
     * get_function_values_from_local_dof_values() for more information.
     */
    template <class InputVector>
    void
    get_function_gradients_from_local_dof_values(
      const InputVector &dof_values,
      std::vector<solution_gradient_type<typename InputVector::value_type>>
        &gradients) const;

    /**
     * Return the symmetrized gradients of the selected vector components of
     * the finite element function characterized by <tt>fe_function</tt> at
     * the quadrature points of the cell, face or subface selected the last
     * time the <tt>reinit</tt> function of the FEValues object was called.
     *
     * The symmetric gradient of a vector field $\mathbf v$ is defined as
     * $\varepsilon(\mathbf v)=\frac 12 (\nabla \mathbf v + \nabla \mathbf
     * v^T)$.
     *
     * @note There is no equivalent function such as
     * FEValuesBase::get_function_symmetric_gradients in the FEValues classes
     * but the information can be obtained from
     * FEValuesBase::get_function_gradients, of course.
     *
     * The data type stored by the output vector must be what you get when you
     * multiply the symmetric gradients of shape functions (i.e., @p
     * symmetric_gradient_type) times the type used to store the values of the
     * unknowns $U_j$ of your finite element vector $U$ (represented by the @p
     * fe_function argument).
     *
     * @dealiiRequiresUpdateFlags{update_gradients}
     */
    template <class InputVector>
    void
    get_function_symmetric_gradients(
      const InputVector &fe_function,
      std::vector<
        solution_symmetric_gradient_type<typename InputVector::value_type>>
        &symmetric_gradients) const;

    /**
     * This function relates to get_function_symmetric_gradients() in the same
     * way as get_function_values_from_local_dof_values() relates to
     * get_function_values(). See the documentation of
     * get_function_values_from_local_dof_values() for more information.
     */
    template <class InputVector>
    void
    get_function_symmetric_gradients_from_local_dof_values(
      const InputVector &dof_values,
      std::vector<
        solution_symmetric_gradient_type<typename InputVector::value_type>>
        &symmetric_gradients) const;

    /**
     * Return the divergence of the selected vector components of the finite
     * element function characterized by <tt>fe_function</tt> at the
     * quadrature points of the cell, face or subface selected the last time
     * the <tt>reinit</tt> function of the FEValues object was called.
     *
     * There is no equivalent function such as
     * FEValuesBase::get_function_divergences in the FEValues classes but the
     * information can be obtained from FEValuesBase::get_function_gradients,
     * of course.
     *
     * The data type stored by the output vector must be what you get when you
     * multiply the divergences of shape functions (i.e., @p divergence_type)
     * times the type used to store the values of the unknowns $U_j$ of your
     * finite element vector $U$ (represented by the @p fe_function argument).
     *
     * @dealiiRequiresUpdateFlags{update_gradients}
     */
    template <class InputVector>
    void
    get_function_divergences(
      const InputVector &fe_function,
      std::vector<solution_divergence_type<typename InputVector::value_type>>
        &divergences) const;

    /**
     * This function relates to get_function_divergences() in the same way
     * as get_function_values_from_local_dof_values() relates to
     * get_function_values(). See the documentation of
     * get_function_values_from_local_dof_values() for more information.
     */
    template <class InputVector>
    void
    get_function_divergences_from_local_dof_values(
      const InputVector &dof_values,
      std::vector<solution_divergence_type<typename InputVector::value_type>>
        &divergences) const;

    /**
     * Return the curl of the selected vector components of the finite element
     * function characterized by <tt>fe_function</tt> at the quadrature points
     * of the cell, face or subface selected the last time the <tt>reinit</tt>
     * function of the FEValues object was called.
     *
     * There is no equivalent function such as
     * FEValuesBase::get_function_curls in the FEValues classes but the
     * information can be obtained from FEValuesBase::get_function_gradients,
     * of course.
     *
     * The data type stored by the output vector must be what you get when you
     * multiply the curls of shape functions (i.e., @p curl_type) times the
     * type used to store the values of the unknowns $U_j$ of your finite
     * element vector $U$ (represented by the @p fe_function argument).
     *
     * @dealiiRequiresUpdateFlags{update_gradients}
     */
    template <class InputVector>
    void
    get_function_curls(
      const InputVector &fe_function,
      std::vector<solution_curl_type<typename InputVector::value_type>> &curls)
      const;

    /**
     * This function relates to get_function_curls() in the same way
     * as get_function_values_from_local_dof_values() relates to
     * get_function_values(). See the documentation of
     * get_function_values_from_local_dof_values() for more information.
     */
    template <class InputVector>
    void
    get_function_curls_from_local_dof_values(
      const InputVector &dof_values,
      std::vector<solution_curl_type<typename InputVector::value_type>> &curls)
      const;

    /**
     * Return the Hessians of the selected vector components of the finite
     * element function characterized by <tt>fe_function</tt> at the
     * quadrature points of the cell, face or subface selected the last time
     * the <tt>reinit</tt> function of the FEValues object was called.
     *
     * This function is the equivalent of the
     * FEValuesBase::get_function_hessians function but it only works on the
     * selected vector components.
     *
     * The data type stored by the output vector must be what you get when you
     * multiply the Hessians of shape functions (i.e., @p hessian_type) times
     * the type used to store the values of the unknowns $U_j$ of your finite
     * element vector $U$ (represented by the @p fe_function argument).
     *
     * @dealiiRequiresUpdateFlags{update_hessians}
     */
    template <class InputVector>
    void
    get_function_hessians(
      const InputVector &fe_function,
      std::vector<solution_hessian_type<typename InputVector::value_type>>
        &hessians) const;

    /**
     * This function relates to get_function_hessians() in the same way
     * as get_function_values_from_local_dof_values() relates to
     * get_function_values(). See the documentation of
     * get_function_values_from_local_dof_values() for more information.
     */
    template <class InputVector>
    void
    get_function_hessians_from_local_dof_values(
      const InputVector &dof_values,
      std::vector<solution_hessian_type<typename InputVector::value_type>>
        &hessians) const;

    /**
     * Return the Laplacians of the selected vector components of the finite
     * element function characterized by <tt>fe_function</tt> at the
     * quadrature points of the cell, face or subface selected the last time
     * the <tt>reinit</tt> function of the FEValues object was called. The
     * Laplacians are the trace of the Hessians.
     *
     * This function is the equivalent of the
     * FEValuesBase::get_function_laplacians function but it only works on the
     * selected vector components.
     *
     * The data type stored by the output vector must be what you get when you
     * multiply the Laplacians of shape functions (i.e., @p laplacian_type)
     * times the type used to store the values of the unknowns $U_j$ of your
     * finite element vector $U$ (represented by the @p fe_function argument).
     *
     * @dealiiRequiresUpdateFlags{update_hessians}
     */
    template <class InputVector>
    void
    get_function_laplacians(
      const InputVector &fe_function,
      std::vector<solution_laplacian_type<typename InputVector::value_type>>
        &laplacians) const;

    /**
     * This function relates to get_function_laplacians() in the same way
     * as get_function_values_from_local_dof_values() relates to
     * get_function_values(). See the documentation of
     * get_function_values_from_local_dof_values() for more information.
     */
    template <class InputVector>
    void
    get_function_laplacians_from_local_dof_values(
      const InputVector &dof_values,
      std::vector<solution_laplacian_type<typename InputVector::value_type>>
        &laplacians) const;

    /**
     * Return the third derivatives of the selected scalar component of the
     * finite element function characterized by <tt>fe_function</tt> at the
     * quadrature points of the cell, face or subface selected the last time
     * the <tt>reinit</tt> function of the FEValues object was called.
     *
     * This function is the equivalent of the
     * FEValuesBase::get_function_third_derivatives function but it only works
     * on the selected scalar component.
     *
     * The data type stored by the output vector must be what you get when you
     * multiply the third derivatives of shape functions (i.e., @p
     * third_derivative_type) times the type used to store the values of the
     * unknowns $U_j$ of your finite element vector $U$ (represented by the @p
     * fe_function argument).
     *
     * @dealiiRequiresUpdateFlags{update_third_derivatives}
     */
    template <class InputVector>
    void
    get_function_third_derivatives(
      const InputVector &fe_function,
      std::vector<
        solution_third_derivative_type<typename InputVector::value_type>>
        &third_derivatives) const;

    /**
     * This function relates to get_function_third_derivatives() in the same way
     * as get_function_values_from_local_dof_values() relates to
     * get_function_values(). See the documentation of
     * get_function_values_from_local_dof_values() for more information.
     */
    template <class InputVector>
    void
    get_function_third_derivatives_from_local_dof_values(
      const InputVector &dof_values,
      std::vector<
        solution_third_derivative_type<typename InputVector::value_type>>
        &third_derivatives) const;

  private:
    /**
     * A pointer to the FEValuesBase object we operate on.
     */
    const SmartPointer<const FEValuesBase<dim, spacedim>> fe_values;

    /**
     * The first component of the vector this view represents of the
     * FEValuesBase object.
     */
    const unsigned int first_vector_component;

    /**
     * Store the data about shape functions.
     */
    std::vector<ShapeFunctionData> shape_function_data;
  };


  template <int rank, int dim, int spacedim = dim>
  class SymmetricTensor;

  /**
   * A class representing a view to a set of <code>(dim*dim + dim)/2</code>
   * components forming a symmetric second-order tensor from a vector-valued
   * finite element. Views are discussed in the
   * @ref vector_valued
   * module.
   *
   * This class allows to query the value and divergence of (components of)
   * shape functions and solutions representing symmetric tensors. The
   * divergence of a symmetric tensor $S_{ij}, 0\le i,j<\text{dim}$ is defined
   * as $d_i = \sum_j \frac{\partial S_{ij}}{\partial x_j}, 0\le
   * i<\text{dim}$, which due to the symmetry of the tensor is also $d_i =
   * \sum_j \frac{\partial S_{ji}}{\partial x_j}$.  In other words, it due to
   * the symmetry of $S$ it does not matter whether we apply the nabla
   * operator by row or by column to get the divergence.
   *
   * You get an object of this type if you apply a
   * FEValuesExtractors::SymmetricTensor to an FEValues, FEFaceValues or
   * FESubfaceValues object.
   *
   * @ingroup feaccess vector_valued
   */
  template <int dim, int spacedim>
  class SymmetricTensor<2, dim, spacedim>
  {
  public:
    /**
     * An alias for the data type of values of the view this class
     * represents. Since we deal with a set of <code>(dim*dim + dim)/2</code>
     * components (i.e. the unique components of a symmetric second-order
     * tensor), the value type is a SymmetricTensor<2,spacedim>.
     */
    using value_type = dealii::SymmetricTensor<2, spacedim>;

    /**
     * An alias for the type of the divergence of the view this class
     * represents. Here, for a set of <code>(dim*dim + dim)/2</code> unique
     * components of the finite element representing a symmetric second-order
     * tensor, the divergence of course is a * <code>Tensor@<1,dim@></code>.
     *
     * See the general discussion of this class for a definition of the
     * divergence.
     */
    using divergence_type = dealii::Tensor<1, spacedim>;

    /**
     * An alias for the data type of the product of a @p Number and the
     * values of the view this class provides. This is the data type of
     * vector components of a finite element field whose degrees of
     * freedom are described by a vector with elements of type @p Number.
     */
    template <typename Number>
    using solution_value_type = typename ProductType<Number, value_type>::type;

    /**
     * An alias for the data type of the product of a @p Number and the
     * divergences of the view this class provides. This is the data type of
     * vector components of a finite element field whose degrees of
     * freedom are described by a vector with elements of type @p Number.
     */
    template <typename Number>
    using solution_divergence_type =
      typename ProductType<Number, divergence_type>::type;


    /**
     * A struct that provides the output type for the product of the value
     * and derivatives of basis functions of the SymmetricTensor view and any @p Number type.
     *
     * @deprecated Use the types defined in the surrounding class instead.
     */
    template <typename Number>
    struct DEAL_II_DEPRECATED OutputType
    {
      /**
       * An alias for the data type of the product of a @p Number and the
       * values of the view the SymmetricTensor class.
       */
      using value_type = typename ProductType<
        Number,
        typename SymmetricTensor<2, dim, spacedim>::value_type>::type;

      /**
       * An alias for the data type of the product of a @p Number and the
       * divergences of the view the SymmetricTensor class.
       */
      using divergence_type = typename ProductType<
        Number,
        typename SymmetricTensor<2, dim, spacedim>::divergence_type>::type;
    };

    /**
     * A structure where for each shape function we pre-compute a bunch of
     * data that will make later accesses much cheaper.
     */
    struct ShapeFunctionData
    {
      /**
       * For each pair (shape function,component within vector), store whether
       * the selected vector component may be nonzero. For primitive shape
       * functions we know for sure whether a certain scalar component of a
       * given shape function is nonzero, whereas for non-primitive shape
       * functions this may not be entirely clear (e.g. for RT elements it
       * depends on the shape of a cell).
       */
      bool is_nonzero_shape_function_component
        [value_type::n_independent_components];

      /**
       * For each pair (shape function, component within vector), store the
       * row index within the shape_values, shape_gradients, and
       * shape_hessians tables (the column index is the quadrature point
       * index). If the shape function is primitive, then we can get this
       * information from the shape_function_to_row_table of the FEValues
       * object; otherwise, we have to work a bit harder to compute this
       * information.
       */
      unsigned int row_index[value_type::n_independent_components];

      /**
       * For each shape function say the following: if only a single entry in
       * is_nonzero_shape_function_component for this shape function is
       * nonzero, then store the corresponding value of row_index and
       * single_nonzero_component_index represents the index between 0 and
       * (dim^2 + dim)/2 for which it is attained. If multiple components are
       * nonzero, then store -1. If no components are nonzero then store -2.
       */
      int single_nonzero_component;

      /**
       * Index of the @p single_nonzero_component .
       */
      unsigned int single_nonzero_component_index;
    };

    /**
     * Default constructor. Creates an invalid object.
     */
    SymmetricTensor();

    /**
     * Constructor for an object that represents <code>(dim*dim +
     * dim)/2</code> components of a FEValuesBase object (or of one of the
     * classes derived from FEValuesBase), representing the unique components
     * comprising a symmetric second- order tensor valued variable.
     *
     * The second argument denotes the index of the first component of the
     * selected symmetric second order tensor.
     */
    SymmetricTensor(const FEValuesBase<dim, spacedim> &fe_values_base,
                    const unsigned int                 first_tensor_component);

    /**
     * Copy constructor. This is not a lightweight object so we don't allow
     * copying and generate a compile-time error if this function is called.
     */
    SymmetricTensor(const SymmetricTensor<2, dim, spacedim> &) = delete;

    /**
     * Move constructor.
     */
    // NOLINTNEXTLINE OSX does not compile with noexcept
    SymmetricTensor(SymmetricTensor<2, dim, spacedim> &&) = default;

    /**
     * Copy operator. This is not a lightweight object so we don't allow
     * copying and generate a compile-time error if this function is called.
     */
    SymmetricTensor &
    operator=(const SymmetricTensor<2, dim, spacedim> &) = delete;

    /**
     * Move assignment operator.
     */
    SymmetricTensor &
    operator=(SymmetricTensor<2, dim, spacedim> &&) noexcept = default;

    /**
     * Return the value of the vector components selected by this view, for
     * the shape function and quadrature point selected by the arguments.
     * Here, since the view represents a vector-valued part of the FEValues
     * object with <code>(dim*dim + dim)/2</code> components (the unique
     * components of a symmetric second-order tensor), the return type is a
     * symmetric tensor of rank 2.
     *
     * @param shape_function Number of the shape function to be evaluated.
     * Note that this number runs from zero to dofs_per_cell, even in the case
     * of an FEFaceValues or FESubfaceValues object.
     *
     * @param q_point Number of the quadrature point at which function is to
     * be evaluated.
     *
     * @dealiiRequiresUpdateFlags{update_values}
     */
    value_type
    value(const unsigned int shape_function, const unsigned int q_point) const;

    /**
     * Return the vector divergence of the vector components selected by this
     * view, for the shape function and quadrature point selected by the
     * arguments.
     *
     * See the general discussion of this class for a definition of the
     * divergence.
     *
     * @note The meaning of the arguments is as documented for the value()
     * function.
     *
     * @dealiiRequiresUpdateFlags{update_gradients}
     */
    divergence_type
    divergence(const unsigned int shape_function,
               const unsigned int q_point) const;

    /**
     * Return the values of the selected vector components of the finite
     * element function characterized by <tt>fe_function</tt> at the
     * quadrature points of the cell, face or subface selected the last time
     * the <tt>reinit</tt> function of the FEValues object was called.
     *
     * This function is the equivalent of the
     * FEValuesBase::get_function_values function but it only works on the
     * selected vector components.
     *
     * The data type stored by the output vector must be what you get when you
     * multiply the values of shape functions (i.e., @p value_type) times the
     * type used to store the values of the unknowns $U_j$ of your finite
     * element vector $U$ (represented by the @p fe_function argument).
     *
     * @dealiiRequiresUpdateFlags{update_values}
     */
    template <class InputVector>
    void
    get_function_values(
      const InputVector &fe_function,
      std::vector<solution_value_type<typename InputVector::value_type>>
        &values) const;

    /**
     * Same as above, but using a vector of local degree-of-freedom values. In
     * other words, instead of extracting the nodal values of the degrees of
     * freedom located on the current cell from a global vector associated with
     * a DoFHandler object (as the function above does), this function instead
     * takes these local nodal values through its first argument. A typical
     * way to obtain such a vector is by calling code such as
     * @code
     *   cell->get_dof_values (dof_values, local_dof_values);
     * @endcode
     * (See DoFCellAccessor::get_dof_values() for more information on this
     * function.) The point of the current function is then that one could
     * modify these local values first, for example by applying a limiter
     * or by ensuring that all nodal values are positive, before evaluating
     * the finite element field that corresponds to these local values on the
     * current cell. Another application is where one wants to postprocess
     * the solution on a cell into a different finite element space on every
     * cell, without actually creating a corresponding DoFHandler -- in that
     * case, all one would compute is a local representation of that
     * postprocessed function, characterized by its nodal values; this function
     * then allows the evaluation of that representation at quadrature points.
     *
     * @param[in] dof_values A vector of local nodal values. This vector must
     *   have a length equal to number of DoFs on the current cell, and must
     *   be ordered in the same order as degrees of freedom are numbered on
     *   the reference cell.
     *
     * @param[out] values A vector of values of the given finite element field,
     *   at the quadrature points on the current object.
     *
     * @tparam InputVector The @p InputVector type must allow creation
     *   of an ArrayView object from it; this is satisfied by the
     *   `std::vector` class, among others.
     */
    template <class InputVector>
    void
    get_function_values_from_local_dof_values(
      const InputVector &dof_values,
      std::vector<solution_value_type<typename InputVector::value_type>>
        &values) const;

    /**
     * Return the divergence of the selected vector components of the finite
     * element function characterized by <tt>fe_function</tt> at the
     * quadrature points of the cell, face or subface selected the last time
     * the <tt>reinit</tt> function of the FEValues object was called.
     *
     * There is no equivalent function such as
     * FEValuesBase::get_function_divergences in the FEValues classes but the
     * information can be obtained from FEValuesBase::get_function_gradients,
     * of course.
     *
     * See the general discussion of this class for a definition of the
     * divergence.
     *
     * The data type stored by the output vector must be what you get when you
     * multiply the divergences of shape functions (i.e., @p divergence_type)
     * times the type used to store the values of the unknowns $U_j$ of your
     * finite element vector $U$ (represented by the @p fe_function argument).
     *
     * @dealiiRequiresUpdateFlags{update_gradients}
     */
    template <class InputVector>
    void
    get_function_divergences(
      const InputVector &fe_function,
      std::vector<solution_divergence_type<typename InputVector::value_type>>
        &divergences) const;

    /**
     * This function relates to get_function_divergences() in the same way
     * as get_function_values_from_local_dof_values() relates to
     * get_function_values(). See the documentation of
     * get_function_values_from_local_dof_values() for more information.
     */
    template <class InputVector>
    void
    get_function_divergences_from_local_dof_values(
      const InputVector &dof_values,
      std::vector<solution_divergence_type<typename InputVector::value_type>>
        &divergences) const;

  private:
    /**
     * A pointer to the FEValuesBase object we operate on.
     */
    const SmartPointer<const FEValuesBase<dim, spacedim>> fe_values;

    /**
     * The first component of the vector this view represents of the
     * FEValuesBase object.
     */
    const unsigned int first_tensor_component;

    /**
     * Store the data about shape functions.
     */
    std::vector<ShapeFunctionData> shape_function_data;
  };


  template <int rank, int dim, int spacedim = dim>
  class Tensor;

  /**
   * A class representing a view to a set of <code>dim*dim</code> components
   * forming a second-order tensor from a vector-valued finite element. Views
   * are discussed in the
   * @ref vector_valued
   * module.
   *
   * This class allows to query the value, gradient and divergence of
   * (components of) shape functions and solutions representing tensors. The
   * divergence of a tensor $T_{ij},\, 0\le i,j<\text{dim}$ is defined as $d_i =
   * \sum_j \frac{\partial T_{ij}}{\partial x_j}, \, 0\le i<\text{dim}$, whereas
   * its gradient is $G_{ijk} = \frac{\partial T_{ij}}{\partial x_k}$.
   *
   * You get an object of this type if you apply a FEValuesExtractors::Tensor
   * to an FEValues, FEFaceValues or FESubfaceValues object.
   *
   * @ingroup feaccess vector_valued
   */
  template <int dim, int spacedim>
  class Tensor<2, dim, spacedim>
  {
  public:
    /**
     * Data type for what you get when you apply an extractor of this kind to
     * a vector-valued finite element.
     */
    using value_type = dealii::Tensor<2, spacedim>;

    /**
     * Data type for taking the divergence of a tensor: a vector.
     */
    using divergence_type = dealii::Tensor<1, spacedim>;

    /**
     * Data type for taking the gradient of a second order tensor: a third order
     * tensor.
     */
    using gradient_type = dealii::Tensor<3, spacedim>;

    /**
     * An alias for the data type of the product of a @p Number and the
     * values of the view this class provides. This is the data type of
     * vector components of a finite element field whose degrees of
     * freedom are described by a vector with elements of type @p Number.
     */
    template <typename Number>
    using solution_value_type = typename ProductType<Number, value_type>::type;

    /**
     * An alias for the data type of the product of a @p Number and the
     * divergences of the view this class provides. This is the data type of
     * vector components of a finite element field whose degrees of
     * freedom are described by a vector with elements of type @p Number.
     */
    template <typename Number>
    using solution_divergence_type =
      typename ProductType<Number, divergence_type>::type;

    /**
     * An alias for the data type of the product of a @p Number and the
     * gradient of the view this class provides. This is the data type of
     * vector components of a finite element field whose degrees of
     * freedom are described by a vector with elements of type @p Number.
     */
    template <typename Number>
    using solution_gradient_type =
      typename ProductType<Number, gradient_type>::type;


    /**
     * A struct that provides the output type for the product of the value
     * and derivatives of basis functions of the Tensor view and any @p Number type.
     *
     * @deprecated Use the types defined in the surrounding class instead.
     */
    template <typename Number>
    struct DEAL_II_DEPRECATED OutputType
    {
      /**
       * An alias for the data type of the product of a @p Number and the
       * values of the view the Tensor class.
       */
      using value_type = typename ProductType<
        Number,
        typename Tensor<2, dim, spacedim>::value_type>::type;

      /**
       * An alias for the data type of the product of a @p Number and the
       * divergences of the view the Tensor class.
       */
      using divergence_type = typename ProductType<
        Number,
        typename Tensor<2, dim, spacedim>::divergence_type>::type;

      /**
       * An alias for the data type of the product of a @p Number and the
       * gradient of the view the Tensor class.
       */
      using gradient_type = typename ProductType<
        Number,
        typename Tensor<2, dim, spacedim>::gradient_type>::type;
    };

    /**
     * A structure where for each shape function we pre-compute a bunch of
     * data that will make later accesses much cheaper.
     */
    struct ShapeFunctionData
    {
      /**
       * For each pair (shape function,component within vector), store whether
       * the selected vector component may be nonzero. For primitive shape
       * functions we know for sure whether a certain scalar component of a
       * given shape function is nonzero, whereas for non-primitive shape
       * functions this may not be entirely clear (e.g. for RT elements it
       * depends on the shape of a cell).
       */
      bool is_nonzero_shape_function_component
        [value_type::n_independent_components];

      /**
       * For each pair (shape function, component within vector), store the
       * row index within the shape_values, shape_gradients, and
       * shape_hessians tables (the column index is the quadrature point
       * index). If the shape function is primitive, then we can get this
       * information from the shape_function_to_row_table of the FEValues
       * object; otherwise, we have to work a bit harder to compute this
       * information.
       */
      unsigned int row_index[value_type::n_independent_components];

      /**
       * For each shape function say the following: if only a single entry in
       * is_nonzero_shape_function_component for this shape function is
       * nonzero, then store the corresponding value of row_index and
       * single_nonzero_component_index represents the index between 0 and
       * (dim^2) for which it is attained. If multiple components are nonzero,
       * then store -1. If no components are nonzero then store -2.
       */
      int single_nonzero_component;

      /**
       * Index of the @p single_nonzero_component .
       */
      unsigned int single_nonzero_component_index;
    };

    /**
     * Default constructor. Creates an invalid object.
     */
    Tensor();

    /**
     * Copy constructor. This is not a lightweight object so we don't allow
     * copying and generate a compile-time error if this function is called.
     */
    Tensor(const Tensor<2, dim, spacedim> &) = delete;

    /**
     * Move constructor.
     */
    // NOLINTNEXTLINE OSX does not compile with noexcept
    Tensor(Tensor<2, dim, spacedim> &&) = default;

    /**
     * Destructor.
     */
    ~Tensor() = default;

    /**
     * Constructor for an object that represents <code>(dim*dim)</code>
     * components of a FEValuesBase object (or of one of the classes derived
     * from FEValuesBase), representing the unique components comprising a
     * second-order tensor valued variable.
     *
     * The second argument denotes the index of the first component of the
     * selected symmetric second order tensor.
     */
    Tensor(const FEValuesBase<dim, spacedim> &fe_values_base,
           const unsigned int                 first_tensor_component);


    /**
     * Copy operator. This is not a lightweight object so we don't allow
     * copying and generate a compile-time error if this function is called.
     */
    Tensor &
    operator=(const Tensor<2, dim, spacedim> &) = delete;

    /**
     * Move assignment operator.
     */
    Tensor &
    operator=(Tensor<2, dim, spacedim> &&) = default; // NOLINT

    /**
     * Return the value of the vector components selected by this view, for
     * the shape function and quadrature point selected by the arguments.
     * Here, since the view represents a vector-valued part of the FEValues
     * object with <code>(dim*dim)</code> components (the unique components of
     * a second-order tensor), the return type is a tensor of rank 2.
     *
     * @param shape_function Number of the shape function to be evaluated.
     * Note that this number runs from zero to dofs_per_cell, even in the case
     * of an FEFaceValues or FESubfaceValues object.
     *
     * @param q_point Number of the quadrature point at which function is to
     * be evaluated.
     *
     * @dealiiRequiresUpdateFlags{update_values}
     */
    value_type
    value(const unsigned int shape_function, const unsigned int q_point) const;

    /**
     * Return the vector divergence of the vector components selected by this
     * view, for the shape function and quadrature point selected by the
     * arguments.
     *
     * See the general discussion of this class for a definition of the
     * divergence.
     *
     * @note The meaning of the arguments is as documented for the value()
     * function.
     *
     * @dealiiRequiresUpdateFlags{update_gradients}
     */
    divergence_type
    divergence(const unsigned int shape_function,
               const unsigned int q_point) const;

    /**
     * Return the gradient (3-rd order tensor) of the vector components selected
     * by this view, for the shape function and quadrature point selected by the
     * arguments.
     *
     * See the general discussion of this class for a definition of the
     * gradient.
     *
     * @note The meaning of the arguments is as documented for the value()
     * function.
     *
     * @dealiiRequiresUpdateFlags{update_gradients}
     */
    gradient_type
    gradient(const unsigned int shape_function,
             const unsigned int q_point) const;

    /**
     * Return the values of the selected vector components of the finite
     * element function characterized by <tt>fe_function</tt> at the
     * quadrature points of the cell, face or subface selected the last time
     * the <tt>reinit</tt> function of the FEValues object was called.
     *
     * This function is the equivalent of the
     * FEValuesBase::get_function_values function but it only works on the
     * selected vector components.
     *
     * The data type stored by the output vector must be what you get when you
     * multiply the values of shape functions (i.e., @p value_type) times the
     * type used to store the values of the unknowns $U_j$ of your finite
     * element vector $U$ (represented by the @p fe_function argument).
     *
     * @dealiiRequiresUpdateFlags{update_values}
     */
    template <class InputVector>
    void
    get_function_values(
      const InputVector &fe_function,
      std::vector<solution_value_type<typename InputVector::value_type>>
        &values) const;

    /**
     * Same as above, but using a vector of local degree-of-freedom values. In
     * other words, instead of extracting the nodal values of the degrees of
     * freedom located on the current cell from a global vector associated with
     * a DoFHandler object (as the function above does), this function instead
     * takes these local nodal values through its first argument. A typical
     * way to obtain such a vector is by calling code such as
     * @code
     *   cell->get_dof_values (dof_values, local_dof_values);
     * @endcode
     * (See DoFCellAccessor::get_dof_values() for more information on this
     * function.) The point of the current function is then that one could
     * modify these local values first, for example by applying a limiter
     * or by ensuring that all nodal values are positive, before evaluating
     * the finite element field that corresponds to these local values on the
     * current cell. Another application is where one wants to postprocess
     * the solution on a cell into a different finite element space on every
     * cell, without actually creating a corresponding DoFHandler -- in that
     * case, all one would compute is a local representation of that
     * postprocessed function, characterized by its nodal values; this function
     * then allows the evaluation of that representation at quadrature points.
     *
     * @param[in] dof_values A vector of local nodal values. This vector must
     *   have a length equal to number of DoFs on the current cell, and must
     *   be ordered in the same order as degrees of freedom are numbered on
     *   the reference cell.
     *
     * @param[out] values A vector of values of the given finite element field,
     *   at the quadrature points on the current object.
     *
     * @tparam InputVector The @p InputVector type must allow creation
     *   of an ArrayView object from it; this is satisfied by the
     *   `std::vector` class, among others.
     */
    template <class InputVector>
    void
    get_function_values_from_local_dof_values(
      const InputVector &dof_values,
      std::vector<solution_value_type<typename InputVector::value_type>>
        &values) const;

    /**
     * Return the divergence of the selected vector components of the finite
     * element function characterized by <tt>fe_function</tt> at the
     * quadrature points of the cell, face or subface selected the last time
     * the <tt>reinit</tt> function of the FEValues object was called.
     *
     * There is no equivalent function such as
     * FEValuesBase::get_function_divergences in the FEValues classes but the
     * information can be obtained from FEValuesBase::get_function_gradients,
     * of course.
     *
     * See the general discussion of this class for a definition of the
     * divergence.
     *
     * The data type stored by the output vector must be what you get when you
     * multiply the divergences of shape functions (i.e., @p divergence_type)
     * times the type used to store the values of the unknowns $U_j$ of your
     * finite element vector $U$ (represented by the @p fe_function argument).
     *
     * @dealiiRequiresUpdateFlags{update_gradients}
     */
    template <class InputVector>
    void
    get_function_divergences(
      const InputVector &fe_function,
      std::vector<solution_divergence_type<typename InputVector::value_type>>
        &divergences) const;

    /**
     * This function relates to get_function_divergences() in the same way
     * as get_function_values_from_local_dof_values() relates to
     * get_function_values(). See the documentation of
     * get_function_values_from_local_dof_values() for more information.
     */
    template <class InputVector>
    void
    get_function_divergences_from_local_dof_values(
      const InputVector &dof_values,
      std::vector<solution_divergence_type<typename InputVector::value_type>>
        &divergences) const;

    /**
     * Return the gradient of the selected vector components of the finite
     * element function characterized by <tt>fe_function</tt> at the
     * quadrature points of the cell, face or subface selected the last time
     * the <tt>reinit</tt> function of the FEValues object was called.
     *
     * See the general discussion of this class for a definition of the
     * gradient.
     *
     * The data type stored by the output vector must be what you get when you
     * multiply the gradients of shape functions (i.e., @p gradient_type)
     * times the type used to store the values of the unknowns $U_j$ of your
     * finite element vector $U$ (represented by the @p fe_function argument).
     *
     * @dealiiRequiresUpdateFlags{update_gradients}
     */
    template <class InputVector>
    void
    get_function_gradients(
      const InputVector &fe_function,
      std::vector<solution_gradient_type<typename InputVector::value_type>>
        &gradients) const;

    /**
     * This function relates to get_function_gradients() in the same way
     * as get_function_values_from_local_dof_values() relates to
     * get_function_values(). See the documentation of
     * get_function_values_from_local_dof_values() for more information.
     */
    template <class InputVector>
    void
    get_function_gradients_from_local_dof_values(
      const InputVector &dof_values,
      std::vector<solution_gradient_type<typename InputVector::value_type>>
        &gradients) const;

  private:
    /**
     * A pointer to the FEValuesBase object we operate on.
     */
    const SmartPointer<const FEValuesBase<dim, spacedim>> fe_values;

    /**
     * The first component of the vector this view represents of the
     * FEValuesBase object.
     */
    const unsigned int first_tensor_component;

    /**
     * Store the data about shape functions.
     */
    std::vector<ShapeFunctionData> shape_function_data;
  };

} // namespace FEValuesViews


namespace internal
{
  namespace FEValuesViews
  {
    /**
     * A class whose specialization is used to define what FEValuesViews
     * object corresponds to the given FEValuesExtractors object.
     */
    template <int dim, int spacedim, typename Extractor>
    struct ViewType
    {};

    /**
     * A class whose specialization is used to define what FEValuesViews
     * object corresponds to the given FEValuesExtractors object.
     *
     * When using FEValuesExtractors::Scalar, the corresponding view is an
     * FEValuesViews::Scalar<dim, spacedim>.
     */
    template <int dim, int spacedim>
    struct ViewType<dim, spacedim, FEValuesExtractors::Scalar>
    {
      using type = typename dealii::FEValuesViews::Scalar<dim, spacedim>;
    };

    /**
     * A class whose specialization is used to define what FEValuesViews
     * object corresponds to the given FEValuesExtractors object.
     *
     * When using FEValuesExtractors::Vector, the corresponding view is an
     * FEValuesViews::Vector<dim, spacedim>.
     */
    template <int dim, int spacedim>
    struct ViewType<dim, spacedim, FEValuesExtractors::Vector>
    {
      using type = typename dealii::FEValuesViews::Vector<dim, spacedim>;
    };

    /**
     * A class whose specialization is used to define what FEValuesViews
     * object corresponds to the given FEValuesExtractors object.
     *
     * When using FEValuesExtractors::Tensor<rank>, the corresponding view is an
     * FEValuesViews::Tensor<rank, dim, spacedim>.
     */
    template <int dim, int spacedim, int rank>
    struct ViewType<dim, spacedim, FEValuesExtractors::Tensor<rank>>
    {
      using type = typename dealii::FEValuesViews::Tensor<rank, dim, spacedim>;
    };

    /**
     * A class whose specialization is used to define what FEValuesViews
     * object corresponds to the given FEValuesExtractors object.
     *
     * When using FEValuesExtractors::SymmetricTensor<rank>, the corresponding
     * view is an FEValuesViews::SymmetricTensor<rank, dim, spacedim>.
     */
    template <int dim, int spacedim, int rank>
    struct ViewType<dim, spacedim, FEValuesExtractors::SymmetricTensor<rank>>
    {
      using type =
        typename dealii::FEValuesViews::SymmetricTensor<rank, dim, spacedim>;
    };

    /**
     * A class objects of which store a collection of FEValuesViews::Scalar,
     * FEValuesViews::Vector, etc object. The FEValuesBase class uses it to
     * generate all possible Views classes upon construction time; we do this
     * at construction time since the Views classes cache some information and
     * are therefore relatively expensive to create.
     */
    template <int dim, int spacedim>
    struct Cache
    {
      /**
       * Caches for scalar and vector, and symmetric second-order tensor
       * valued views.
       */
      std::vector<dealii::FEValuesViews::Scalar<dim, spacedim>> scalars;
      std::vector<dealii::FEValuesViews::Vector<dim, spacedim>> vectors;
      std::vector<dealii::FEValuesViews::SymmetricTensor<2, dim, spacedim>>
        symmetric_second_order_tensors;
      std::vector<dealii::FEValuesViews::Tensor<2, dim, spacedim>>
        second_order_tensors;

      /**
       * Constructor.
       */
      Cache(const FEValuesBase<dim, spacedim> &fe_values);
    };
  } // namespace FEValuesViews
} // namespace internal

namespace FEValuesViews
{
  /**
   * A templated alias that associates to a given Extractor class
   * the corresponding view in FEValuesViews.
   */
  template <int dim, int spacedim, typename Extractor>
  using View = typename dealii::internal::FEValuesViews::
    ViewType<dim, spacedim, Extractor>::type;
} // namespace FEValuesViews


/**
 * FEValues, FEFaceValues and FESubfaceValues objects are interfaces to finite
 * element and mapping classes on the one hand side, to cells and quadrature
 * rules on the other side. They allow to evaluate values or derivatives of
 * shape functions at the quadrature points of a quadrature formula when
 * projected by a mapping from the unit cell onto a cell in real space. The
 * reason for this abstraction is possible optimization: Depending on the type
 * of finite element and mapping, some values can be computed once on the unit
 * cell. Others must be computed on each cell, but maybe computation of
 * several values at the same time offers ways for optimization. Since this
 * interplay may be complex and depends on the actual finite element, it
 * cannot be left to the applications programmer.
 *
 * FEValues, FEFaceValues and FESubfaceValues provide only data handling:
 * computations are left to objects of type Mapping and FiniteElement. These
 * provide functions <tt>get_*_data</tt> and <tt>fill_*_values</tt> which are
 * called by the constructor and <tt>reinit</tt> functions of
 * <tt>FEValues*</tt>, respectively.
 *
 * <h3>General usage</h3>
 *
 * Usually, an object of <tt>FEValues*</tt> is used in integration loops over
 * all cells of a triangulation (or faces of cells). To take full advantage of
 * the optimization features, it should be constructed before the loop so that
 * information that does not depend on the location and shape of cells can be
 * computed once and for all (this includes, for example, the values of shape
 * functions at quadrature points for the most common elements: we can
 * evaluate them on the unit cell and they will be the same when mapped to the
 * real cell). Then, in the loop over all cells, it must be re-initialized for
 * each grid cell to compute that part of the information that changes
 * depending on the actual cell (for example, the gradient of shape functions
 * equals the gradient on the unit cell -- which can be computed once and for
 * all -- times the Jacobian matrix of the mapping between unit and real cell,
 * which needs to be recomputed for each cell).
 *
 * A typical piece of code, adding up local contributions to the Laplace
 * matrix looks like this:
 *
 * @code
 * FEValues values (mapping, finite_element, quadrature, flags);
 * for (const auto &cell : dof_handler.active_cell_iterators())
 *   {
 *     values.reinit(cell);
 *     for (unsigned int q=0; q<quadrature.size(); ++q)
 *       for (unsigned int i=0; i<finite_element.n_dofs_per_cell(); ++i)
 *         for (unsigned int j=0; j<finite_element.n_dofs_per_cell(); ++j)
 *           A(i,j) += fe_values.shape_value(i,q) *
 *                     fe_values.shape_value(j,q) *
 *                     fe_values.JxW(q);
 *     ...
 *   }
 * @endcode
 *
 * The individual functions used here are described below. Note that by
 * design, the order of quadrature points used inside the FEValues object is
 * the same as defined by the quadrature formula passed to the constructor of
 * the FEValues object above.
 *
 * <h3>Member functions</h3>
 *
 * The functions of this class fall into different categories:
 * <ul>
 * <li> shape_value(), shape_grad(), etc: return one of the values of this
 * object at a time. These functions are inlined, so this is the suggested
 * access to all finite element values. There should be no loss in performance
 * with an optimizing compiler. If the finite element is vector valued, then
 * these functions return the only non-zero component of the requested shape
 * function. However, some finite elements have shape functions that have more
 * than one non-zero component (we call them non-"primitive"), and in this
 * case this set of functions will throw an exception since they cannot
 * generate a useful result. Rather, use the next set of functions.
 *
 * <li> shape_value_component(), shape_grad_component(), etc: This is the same
 * set of functions as above, except that for vector valued finite elements
 * they return only one vector component. This is useful for elements of which
 * shape functions have more than one non-zero component, since then the above
 * functions cannot be used, and you have to walk over all (or only the non-
 * zero) components of the shape function using this set of functions.
 *
 * <li> get_function_values(), get_function_gradients(), etc.: Compute a
 * finite element function or its derivative in quadrature points.
 *
 * <li> reinit: initialize the FEValues object for a certain cell. This
 * function is not in the present class but only in the derived classes and
 * has a variable call syntax. See the docs for the derived classes for more
 * information.
 * </ul>
 *
 *
 * <h3>Internals about the implementation</h3>
 *
 * The mechanisms by which this class work are discussed on the page on
 * @ref UpdateFlags "Update flags"
 * and about the
 * @ref FE_vs_Mapping_vs_FEValues "How Mapping, FiniteElement, and FEValues work together".
 *
 *
 * @ingroup feaccess
 */
template <int dim, int spacedim>
class FEValuesBase : public Subscriptor
{
public:
  /**
   * Dimension in which this object operates.
   */
  static constexpr unsigned int dimension = dim;

  /**
   * Dimension of the space in which this object operates.
   */
  static constexpr unsigned int space_dimension = spacedim;

  /**
   * Number of quadrature points of the current object. Its value is
   * initialized by the value of max_n_quadrature_points and is updated,
   * e.g., if FEFaceValues::reinit() is called for a new cell/face.
   *
   * @note The default value equals to the value of max_n_quadrature_points.
   */
  const unsigned int n_quadrature_points;

  /**
   * Maximum number of quadrature points. This value might be different from
   * n_quadrature_points, e.g., if a QCollection with different face quadrature
   * rules has been passed to initialize FEFaceValues.
   *
   * This is mostly useful to initialize arrays to allocate the maximum amount
   * of memory that may be used when re-sizing later on to a the current
   * number of quadrature points given by n_quadrature_points.
   */
  const unsigned int max_n_quadrature_points;

  /**
   * Number of shape functions per cell. If we use this base class to evaluate
   * a finite element on faces of cells, this is still the number of degrees
   * of freedom per cell, not per face.
   */
  const unsigned int dofs_per_cell;


  /**
   * Constructor. Set up the array sizes with <tt>n_q_points</tt> quadrature
   * points, <tt>dofs_per_cell</tt> trial functions per cell and with the
   * given pattern to update the fields when the <tt>reinit</tt> function of
   * the derived classes is called. The fields themselves are not set up, this
   * must happen in the constructor of the derived class.
   */
  FEValuesBase(const unsigned int                  n_q_points,
               const unsigned int                  dofs_per_cell,
               const UpdateFlags                   update_flags,
               const Mapping<dim, spacedim> &      mapping,
               const FiniteElement<dim, spacedim> &fe);

  /**
   * The copy assignment is deleted since objects of this class are not
   * copyable.
   */
  FEValuesBase &
  operator=(const FEValuesBase &) = delete;

  /**
   * The copy constructor is deleted since objects of this class are not
   * copyable.
   */
  FEValuesBase(const FEValuesBase &) = delete;

  /**
   * Destructor.
   */
  virtual ~FEValuesBase() override;


  /// @name Access to shape function values
  ///
  /// These fields are filled by the finite element.
  /** @{ */

  /**
   * Value of a shape function at a quadrature point on the cell, face or
   * subface selected the last time the <tt>reinit</tt> function of the
   * derived class was called.
   *
   * If the shape function is vector-valued, then this returns the only non-
   * zero component. If the shape function has more than one non-zero
   * component (i.e. it is not primitive), then throw an exception of type
   * ExcShapeFunctionNotPrimitive. In that case, use the
   * shape_value_component() function.
   *
   * @param function_no Number of the shape function to be evaluated. Note
   * that this number runs from zero to dofs_per_cell, even in the case of an
   * FEFaceValues or FESubfaceValues object.
   *
   * @param point_no Number of the quadrature point at which function is to be
   * evaluated
   *
   * @dealiiRequiresUpdateFlags{update_values}
   */
  const double &
  shape_value(const unsigned int function_no,
              const unsigned int point_no) const;

  /**
   * Compute one vector component of the value of a shape function at a
   * quadrature point. If the finite element is scalar, then only component
   * zero is allowed and the return value equals that of the shape_value()
   * function. If the finite element is vector valued but all shape functions
   * are primitive (i.e. they are non-zero in only one component), then the
   * value returned by shape_value() equals that of this function for exactly
   * one component. This function is therefore only of greater interest if the
   * shape function is not primitive, but then it is necessary since the other
   * function cannot be used.
   *
   * @param function_no Number of the shape function to be evaluated.
   *
   * @param point_no Number of the quadrature point at which function is to be
   * evaluated.
   *
   * @param component vector component to be evaluated.
   *
   * @dealiiRequiresUpdateFlags{update_values}
   */
  double
  shape_value_component(const unsigned int function_no,
                        const unsigned int point_no,
                        const unsigned int component) const;

  /**
   * Compute the gradient of the <tt>function_no</tt>th shape function at the
   * <tt>quadrature_point</tt>th quadrature point with respect to real cell
   * coordinates.  If you want to get the derivative in one of the coordinate
   * directions, use the appropriate function of the Tensor class to extract
   * one component of the Tensor returned by this function. Since only a
   * reference to the gradient's value is returned, there should be no major
   * performance drawback.
   *
   * If the shape function is vector-valued, then this returns the only non-
   * zero component. If the shape function has more than one non-zero
   * component (i.e. it is not primitive), then it will throw an exception of
   * type ExcShapeFunctionNotPrimitive. In that case, use the
   * shape_grad_component() function.
   *
   * The same holds for the arguments of this function as for the
   * shape_value() function.
   *
   * @param function_no Number of the shape function to be evaluated.
   *
   * @param quadrature_point Number of the quadrature point at which function
   * is to be evaluated.
   *
   * @dealiiRequiresUpdateFlags{update_gradients}
   */
  const Tensor<1, spacedim> &
  shape_grad(const unsigned int function_no,
             const unsigned int quadrature_point) const;

  /**
   * Return one vector component of the gradient of a shape function at a
   * quadrature point. If the finite element is scalar, then only component
   * zero is allowed and the return value equals that of the shape_grad()
   * function. If the finite element is vector valued but all shape functions
   * are primitive (i.e. they are non-zero in only one component), then the
   * value returned by shape_grad() equals that of this function for exactly
   * one component. This function is therefore only of greater interest if the
   * shape function is not primitive, but then it is necessary since the other
   * function cannot be used.
   *
   * The same holds for the arguments of this function as for the
   * shape_value_component() function.
   *
   * @dealiiRequiresUpdateFlags{update_gradients}
   */
  Tensor<1, spacedim>
  shape_grad_component(const unsigned int function_no,
                       const unsigned int point_no,
                       const unsigned int component) const;

  /**
   * Second derivatives of the <tt>function_no</tt>th shape function at the
   * <tt>point_no</tt>th quadrature point with respect to real cell
   * coordinates. If you want to get the derivatives in one of the coordinate
   * directions, use the appropriate function of the Tensor class to extract
   * one component. Since only a reference to the hessian values is returned,
   * there should be no major performance drawback.
   *
   * If the shape function is vector-valued, then this returns the only non-
   * zero component. If the shape function has more than one non-zero
   * component (i.e. it is not primitive), then throw an exception of type
   * ExcShapeFunctionNotPrimitive. In that case, use the
   * shape_hessian_component() function.
   *
   * The same holds for the arguments of this function as for the
   * shape_value() function.
   *
   * @dealiiRequiresUpdateFlags{update_hessians}
   */
  const Tensor<2, spacedim> &
  shape_hessian(const unsigned int function_no,
                const unsigned int point_no) const;

  /**
   * Return one vector component of the hessian of a shape function at a
   * quadrature point. If the finite element is scalar, then only component
   * zero is allowed and the return value equals that of the shape_hessian()
   * function. If the finite element is vector valued but all shape functions
   * are primitive (i.e. they are non-zero in only one component), then the
   * value returned by shape_hessian() equals that of this function for
   * exactly one component. This function is therefore only of greater
   * interest if the shape function is not primitive, but then it is necessary
   * since the other function cannot be used.
   *
   * The same holds for the arguments of this function as for the
   * shape_value_component() function.
   *
   * @dealiiRequiresUpdateFlags{update_hessians}
   */
  Tensor<2, spacedim>
  shape_hessian_component(const unsigned int function_no,
                          const unsigned int point_no,
                          const unsigned int component) const;

  /**
   * Third derivatives of the <tt>function_no</tt>th shape function at the
   * <tt>point_no</tt>th quadrature point with respect to real cell
   * coordinates. If you want to get the 3rd derivatives in one of the
   * coordinate directions, use the appropriate function of the Tensor class
   * to extract one component. Since only a reference to the 3rd derivative
   * values is returned, there should be no major performance drawback.
   *
   * If the shape function is vector-valued, then this returns the only non-
   * zero component. If the shape function has more than one non-zero
   * component (i.e. it is not primitive), then throw an exception of type
   * ExcShapeFunctionNotPrimitive. In that case, use the
   * shape_3rdderivative_component() function.
   *
   * The same holds for the arguments of this function as for the
   * shape_value() function.
   *
   * @dealiiRequiresUpdateFlags{update_3rd_derivatives}
   */
  const Tensor<3, spacedim> &
  shape_3rd_derivative(const unsigned int function_no,
                       const unsigned int point_no) const;

  /**
   * Return one vector component of the third derivative of a shape function
   * at a quadrature point. If the finite element is scalar, then only
   * component zero is allowed and the return value equals that of the
   * shape_3rdderivative() function. If the finite element is vector valued
   * but all shape functions are primitive (i.e. they are non-zero in only one
   * component), then the value returned by shape_3rdderivative() equals that
   * of this function for exactly one component. This function is therefore
   * only of greater interest if the shape function is not primitive, but then
   * it is necessary since the other function cannot be used.
   *
   * The same holds for the arguments of this function as for the
   * shape_value_component() function.
   *
   * @dealiiRequiresUpdateFlags{update_3rd_derivatives}
   */
  Tensor<3, spacedim>
  shape_3rd_derivative_component(const unsigned int function_no,
                                 const unsigned int point_no,
                                 const unsigned int component) const;

  /** @} */
  /// @name Access to values of global finite element fields
  /** @{ */

  /**
   * Return the values of a finite element function restricted to the current
   * cell, face or subface selected the last time the <tt>reinit</tt> function
   * of the derived class was called, at the quadrature points.
   *
   * If the present cell is not active then values are interpolated to the
   * current cell and point values are computed from that.
   *
   * This function may only be used if the finite element in use is a scalar
   * one, i.e. has only one vector component.  To get values of multi-
   * component elements, there is another get_function_values() below,
   * returning a vector of vectors of results.
   *
   * @param[in] fe_function A vector of values that describes (globally) the
   * finite element function that this function should evaluate at the
   * quadrature points of the current cell.
   *
   * @param[out] values The values of the function specified by fe_function at
   * the quadrature points of the current cell.  The object is assume to
   * already have the correct size. The data type stored by this output vector
   * must be what you get when you multiply the values of shape function times
   * the type used to store the values of the unknowns $U_j$ of your finite
   * element vector $U$ (represented by the @p fe_function argument). This
   * happens to be equal to the type of the elements of the solution vector.
   *
   * @post <code>values[q]</code> will contain the value of the field
   * described by fe_function at the $q$th quadrature point.
   *
   * @note The actual data type of the input vector may be either a
   * Vector&lt;T&gt;, BlockVector&lt;T&gt;, or one of the PETSc or Trilinos
   * vector wrapper classes. It represents a global vector of DoF values
   * associated with the DoFHandler object with which this FEValues object was
   * last initialized.
   *
   * @dealiiRequiresUpdateFlags{update_values}
   */
  template <class InputVector>
  void
  get_function_values(
    const InputVector &                            fe_function,
    std::vector<typename InputVector::value_type> &values) const;

  /**
   * This function does the same as the other get_function_values(), but
   * applied to multi-component (vector-valued) elements. The meaning of the
   * arguments is as explained there.
   *
   * @post <code>values[q]</code> is a vector of values of the field described
   * by fe_function at the $q$th quadrature point. The size of the vector
   * accessed by <code>values[q]</code> equals the number of components of the
   * finite element, i.e. <code>values[q](c)</code> returns the value of the
   * $c$th vector component at the $q$th quadrature point.
   *
   * @dealiiRequiresUpdateFlags{update_values}
   */
  template <class InputVector>
  void
  get_function_values(
    const InputVector &                                    fe_function,
    std::vector<Vector<typename InputVector::value_type>> &values) const;

  /**
   * Generate function values from an arbitrary vector. This function
   * does in essence the same as the first function of this name above,
   * except that it does not make the assumption that the input vector
   * corresponds to a DoFHandler that describes the unknowns of a finite
   * element field (and for which we would then assume that
   * `fe_function.size() == dof_handler.n_dofs()`). Rather, the nodal
   * values corresponding to the current cell are elements of an otherwise
   * arbitrary vector, and these elements are indexed by the second
   * argument to this function. What the rest of the `fe_function` input
   * argument corresponds to is of no consequence to this function.
   *
   * Given this, the function above corresponds to passing `fe_function`
   * as first argument to the current function, and using the
   * `local_dof_indices` array that results from the following call as
   * second argument to the current function:
   * @code
   *   cell->get_dof_indices (local_dof_indices);
   * @endcode
   * (See DoFCellAccessor::get_dof_indices() for more information.)
   *
   * Likewise, the function above is equivalent to calling
   * @code
   *   cell->get_dof_values (fe_function, local_dof_values);
   * @endcode
   * and then calling the current function with `local_dof_values` as
   * first argument, and an array with indices `{0,...,fe.dofs_per_cell-1}`
   * as second argument.
   *
   * The point of the current function is that one sometimes wants to
   * evaluate finite element functions at quadrature points with nodal
   * values that are not stored in a global vector -- for example, one could
   * modify these local values first, such as by applying a limiter
   * or by ensuring that all nodal values are positive, before evaluating
   * the finite element field that corresponds to these local values on the
   * current cell. Another application is where one wants to postprocess
   * the solution on a cell into a different finite element space on every
   * cell, without actually creating a corresponding DoFHandler -- in that
   * case, all one would compute is a local representation of that
   * postprocessed function, characterized by its nodal values; this function
   * then allows the evaluation of that representation at quadrature points.
   *
   * @param[in] fe_function A vector of nodal values. This vector can have
   *   an arbitrary size, as long as all elements index by `indices` can
   *   actually be accessed.
   *
   * @param[in] indices A vector of indices into `fe_function`. This vector
   *   must have length equal to the number of degrees of freedom on the
   *   current cell, and must identify elements in `fe_function` in the
   *   order in which degrees of freedom are indexed on the reference cell.
   *
   * @param[out] values A vector of values of the given finite element field,
   *   at the quadrature points on the current object.
   *
   * @dealiiRequiresUpdateFlags{update_values}
   */
  template <class InputVector>
  void
  get_function_values(
    const InputVector &                             fe_function,
    const ArrayView<const types::global_dof_index> &indices,
    std::vector<typename InputVector::value_type> & values) const;

  /**
   * Generate vector function values from an arbitrary vector.
   *
   * This function corresponds to the previous one, just for the vector-valued
   * case.
   *
   * @dealiiRequiresUpdateFlags{update_values}
   */
  template <class InputVector>
  void
  get_function_values(
    const InputVector &                                    fe_function,
    const ArrayView<const types::global_dof_index> &       indices,
    std::vector<Vector<typename InputVector::value_type>> &values) const;


  /**
   * Generate vector function values from an arbitrary vector. This
   * function is similar to the previous one, but the `indices`
   * vector may also be a multiple of the number of dofs per
   * cell. Then, the vectors in <tt>value</tt> should allow for the same
   * multiple of the components of the finite element.
   *
   * Depending on the value of the last argument, the outer vector of
   * <tt>values</tt> has either the length of the quadrature rule
   * (<tt>quadrature_points_fastest == false</tt>) or the length of components
   * to be filled <tt>quadrature_points_fastest == true</tt>. If <tt>p</tt> is
   * the current quadrature point number and <tt>i</tt> is the vector
   * component of the solution desired, the access to <tt>values</tt> is
   * <tt>values[p][i]</tt> if <tt>quadrature_points_fastest == false</tt>, and
   * <tt>values[i][p]</tt> otherwise.
   *
   * Since this function allows for fairly general combinations of argument
   * sizes, be aware that the checks on the arguments may not detect errors.
   *
   * @dealiiRequiresUpdateFlags{update_values}
   */
  template <class InputVector>
  void
  get_function_values(
    const InputVector &                                      fe_function,
    const ArrayView<const types::global_dof_index> &         indices,
    ArrayView<std::vector<typename InputVector::value_type>> values,
    const bool quadrature_points_fastest) const;

  /** @} */
  /// @name Access to derivatives of global finite element fields
  /** @{ */

  /**
   * Compute the gradients of a finite element at the quadrature points of a
   * cell. This function is the equivalent of the corresponding
   * get_function_values() function (see there for more information) but
   * evaluates the finite element field's gradient instead of its value.
   *
   * This function may only be used if the finite element in use is a scalar
   * one, i.e. has only one vector component. There is a corresponding
   * function of the same name for vector-valued finite elements.
   *
   * @param[in] fe_function A vector of values that describes (globally) the
   * finite element function that this function should evaluate at the
   * quadrature points of the current cell.
   *
   * @param[out] gradients The gradients of the function specified by
   * fe_function at the quadrature points of the current cell.  The gradients
   * are computed in real space (as opposed to on the unit cell).  The object
   * is assume to already have the correct size. The data type stored by this
   * output vector must be what you get when you multiply the gradients of
   * shape function times the type used to store the values of the unknowns
   * $U_j$ of your finite element vector $U$ (represented by the @p
   * fe_function argument).
   *
   * @post <code>gradients[q]</code> will contain the gradient of the field
   * described by fe_function at the $q$th quadrature point.
   * <code>gradients[q][d]</code> represents the derivative in coordinate
   * direction $d$ at quadrature point $q$.
   *
   * @note The actual data type of the input vector may be either a
   * Vector&lt;T&gt;, BlockVector&lt;T&gt;, or one of the PETSc or Trilinos
   * vector wrapper classes. It represents a global vector of DoF values
   * associated with the DoFHandler object with which this FEValues object was
   * last initialized.
   *
   * @dealiiRequiresUpdateFlags{update_gradients}
   */
  template <class InputVector>
  void
  get_function_gradients(
    const InputVector &fe_function,
    std::vector<Tensor<1, spacedim, typename InputVector::value_type>>
      &gradients) const;

  /**
   * This function does the same as the other get_function_gradients(), but
   * applied to multi-component (vector-valued) elements. The meaning of the
   * arguments is as explained there.
   *
   * @post <code>gradients[q]</code> is a vector of gradients of the field
   * described by fe_function at the $q$th quadrature point. The size of the
   * vector accessed by <code>gradients[q]</code> equals the number of
   * components of the finite element, i.e. <code>gradients[q][c]</code>
   * returns the gradient of the $c$th vector component at the $q$th
   * quadrature point. Consequently, <code>gradients[q][c][d]</code> is the
   * derivative in coordinate direction $d$ of the $c$th vector component of
   * the vector field at quadrature point $q$ of the current cell.
   *
   * @dealiiRequiresUpdateFlags{update_gradients}
   */
  template <class InputVector>
  void
  get_function_gradients(
    const InputVector &fe_function,
    std::vector<
      std::vector<Tensor<1, spacedim, typename InputVector::value_type>>>
      &gradients) const;

  /**
   * This function relates to the first of the get_function_gradients() function
   * above in the same way as the get_function_values() with similar arguments
   * relates to the first of the get_function_values() functions. See there for
   * more information.
   *
   * @dealiiRequiresUpdateFlags{update_gradients}
   */
  template <class InputVector>
  void
  get_function_gradients(
    const InputVector &                             fe_function,
    const ArrayView<const types::global_dof_index> &indices,
    std::vector<Tensor<1, spacedim, typename InputVector::value_type>>
      &gradients) const;

  /**
   * This function relates to the first of the get_function_gradients() function
   * above in the same way as the get_function_values() with similar arguments
   * relates to the first of the get_function_values() functions. See there for
   * more information.
   *
   * @dealiiRequiresUpdateFlags{update_gradients}
   */
  template <class InputVector>
  void
  get_function_gradients(
    const InputVector &                             fe_function,
    const ArrayView<const types::global_dof_index> &indices,
    ArrayView<
      std::vector<Tensor<1, spacedim, typename InputVector::value_type>>>
               gradients,
    const bool quadrature_points_fastest = false) const;

  /** @} */
  /// @name Access to second derivatives
  ///
  /// Hessian matrices and Laplacians of global finite element fields
  /** @{ */

  /**
   * Compute the tensor of second derivatives of a finite element at the
   * quadrature points of a cell. This function is the equivalent of the
   * corresponding get_function_values() function (see there for more
   * information) but evaluates the finite element field's second derivatives
   * instead of its value.
   *
   * This function may only be used if the finite element in use is a scalar
   * one, i.e. has only one vector component. There is a corresponding
   * function of the same name for vector-valued finite elements.
   *
   * @param[in] fe_function A vector of values that describes (globally) the
   * finite element function that this function should evaluate at the
   * quadrature points of the current cell.
   *
   * @param[out] hessians The Hessians of the function specified by
   * fe_function at the quadrature points of the current cell.  The Hessians
   * are computed in real space (as opposed to on the unit cell).  The object
   * is assume to already have the correct size. The data type stored by this
   * output vector must be what you get when you multiply the Hessians of
   * shape function times the type used to store the values of the unknowns
   * $U_j$ of your finite element vector $U$ (represented by the @p
   * fe_function argument).
   *
   * @post <code>hessians[q]</code> will contain the Hessian of the field
   * described by fe_function at the $q$th quadrature point.
   * <code>hessians[q][i][j]</code> represents the $(i,j)$th component of the
   * matrix of second derivatives at quadrature point $q$.
   *
   * @note The actual data type of the input vector may be either a
   * Vector&lt;T&gt;, BlockVector&lt;T&gt;, or one of the PETSc or Trilinos
   * vector wrapper classes. It represents a global vector of DoF values
   * associated with the DoFHandler object with which this FEValues object was
   * last initialized.
   *
   * @dealiiRequiresUpdateFlags{update_hessians}
   */
  template <class InputVector>
  void
  get_function_hessians(
    const InputVector &fe_function,
    std::vector<Tensor<2, spacedim, typename InputVector::value_type>>
      &hessians) const;

  /**
   * This function does the same as the other get_function_hessians(), but
   * applied to multi-component (vector-valued) elements. The meaning of the
   * arguments is as explained there.
   *
   * @post <code>hessians[q]</code> is a vector of Hessians of the field
   * described by fe_function at the $q$th quadrature point. The size of the
   * vector accessed by <code>hessians[q]</code> equals the number of
   * components of the finite element, i.e. <code>hessians[q][c]</code>
   * returns the Hessian of the $c$th vector component at the $q$th quadrature
   * point. Consequently, <code>hessians[q][c][i][j]</code> is the $(i,j)$th
   * component of the matrix of second derivatives of the $c$th vector
   * component of the vector field at quadrature point $q$ of the current
   * cell.
   *
   * @dealiiRequiresUpdateFlags{update_hessians}
   */
  template <class InputVector>
  void
  get_function_hessians(
    const InputVector &fe_function,
    std::vector<
      std::vector<Tensor<2, spacedim, typename InputVector::value_type>>>
      &        hessians,
    const bool quadrature_points_fastest = false) const;

  /**
   * This function relates to the first of the get_function_hessians() function
   * above in the same way as the get_function_values() with similar arguments
   * relates to the first of the get_function_values() functions. See there for
   * more information.
   *
   * @dealiiRequiresUpdateFlags{update_hessians}
   */
  template <class InputVector>
  void
  get_function_hessians(
    const InputVector &                             fe_function,
    const ArrayView<const types::global_dof_index> &indices,
    std::vector<Tensor<2, spacedim, typename InputVector::value_type>>
      &hessians) const;

  /**
   * This function relates to the first of the get_function_hessians() function
   * above in the same way as the get_function_values() with similar arguments
   * relates to the first of the get_function_values() functions. See there for
   * more information.
   *
   * @dealiiRequiresUpdateFlags{update_hessians}
   */
  template <class InputVector>
  void
  get_function_hessians(
    const InputVector &                             fe_function,
    const ArrayView<const types::global_dof_index> &indices,
    ArrayView<
      std::vector<Tensor<2, spacedim, typename InputVector::value_type>>>
               hessians,
    const bool quadrature_points_fastest = false) const;

  /**
   * Compute the (scalar) Laplacian (i.e. the trace of the tensor of second
   * derivatives) of a finite element at the quadrature points of a cell. This
   * function is the equivalent of the corresponding get_function_values()
   * function (see there for more information) but evaluates the finite
   * element field's second derivatives instead of its value.
   *
   * This function may only be used if the finite element in use is a scalar
   * one, i.e. has only one vector component. There is a corresponding
   * function of the same name for vector-valued finite elements.
   *
   * @param[in] fe_function A vector of values that describes (globally) the
   * finite element function that this function should evaluate at the
   * quadrature points of the current cell.
   *
   * @param[out] laplacians The Laplacians of the function specified by
   * fe_function at the quadrature points of the current cell.  The Laplacians
   * are computed in real space (as opposed to on the unit cell).  The object
   * is assume to already have the correct size. The data type stored by this
   * output vector must be what you get when you multiply the Laplacians of
   * shape function times the type used to store the values of the unknowns
   * $U_j$ of your finite element vector $U$ (represented by the @p
   * fe_function argument). This happens to be equal to the type of the
   * elements of the input vector.
   *
   * @post <code>laplacians[q]</code> will contain the Laplacian of the field
   * described by fe_function at the $q$th quadrature point.
   *
   * @post For each component of the output vector, there holds
   * <code>laplacians[q]=trace(hessians[q])</code>, where <tt>hessians</tt>
   * would be the output of the get_function_hessians() function.
   *
   * @note The actual data type of the input vector may be either a
   * Vector&lt;T&gt;, BlockVector&lt;T&gt;, or one of the PETSc or Trilinos
   * vector wrapper classes. It represents a global vector of DoF values
   * associated with the DoFHandler object with which this FEValues object was
   * last initialized.
   *
   * @dealiiRequiresUpdateFlags{update_hessians}
   */
  template <class InputVector>
  void
  get_function_laplacians(
    const InputVector &                            fe_function,
    std::vector<typename InputVector::value_type> &laplacians) const;

  /**
   * This function does the same as the other get_function_laplacians(), but
   * applied to multi-component (vector-valued) elements. The meaning of the
   * arguments is as explained there.
   *
   * @post <code>laplacians[q]</code> is a vector of Laplacians of the field
   * described by fe_function at the $q$th quadrature point. The size of the
   * vector accessed by <code>laplacians[q]</code> equals the number of
   * components of the finite element, i.e. <code>laplacians[q][c]</code>
   * returns the Laplacian of the $c$th vector component at the $q$th
   * quadrature point.
   *
   * @post For each component of the output vector, there holds
   * <code>laplacians[q][c]=trace(hessians[q][c])</code>, where
   * <tt>hessians</tt> would be the output of the get_function_hessians()
   * function.
   *
   * @dealiiRequiresUpdateFlags{update_hessians}
   */
  template <class InputVector>
  void
  get_function_laplacians(
    const InputVector &                                    fe_function,
    std::vector<Vector<typename InputVector::value_type>> &laplacians) const;

  /**
   * This function relates to the first of the get_function_laplacians()
   * function above in the same way as the get_function_values() with similar
   * arguments relates to the first of the get_function_values() functions. See
   * there for more information.
   *
   * @dealiiRequiresUpdateFlags{update_hessians}
   */
  template <class InputVector>
  void
  get_function_laplacians(
    const InputVector &                             fe_function,
    const ArrayView<const types::global_dof_index> &indices,
    std::vector<typename InputVector::value_type> & laplacians) const;

  /**
   * This function relates to the first of the get_function_laplacians()
   * function above in the same way as the get_function_values() with similar
   * arguments relates to the first of the get_function_values() functions. See
   * there for more information.
   *
   * @dealiiRequiresUpdateFlags{update_hessians}
   */
  template <class InputVector>
  void
  get_function_laplacians(
    const InputVector &                                    fe_function,
    const ArrayView<const types::global_dof_index> &       indices,
    std::vector<Vector<typename InputVector::value_type>> &laplacians) const;

  /**
   * This function relates to the first of the get_function_laplacians()
   * function above in the same way as the get_function_values() with similar
   * arguments relates to the first of the get_function_values() functions. See
   * there for more information.
   *
   * @dealiiRequiresUpdateFlags{update_hessians}
   */
  template <class InputVector>
  void
  get_function_laplacians(
    const InputVector &                                         fe_function,
    const ArrayView<const types::global_dof_index> &            indices,
    std::vector<std::vector<typename InputVector::value_type>> &laplacians,
    const bool quadrature_points_fastest = false) const;

  /** @} */
  /// @name Access to third derivatives of global finite element fields
  /** @{ */

  /**
   * Compute the tensor of third derivatives of a finite element at the
   * quadrature points of a cell. This function is the equivalent of the
   * corresponding get_function_values() function (see there for more
   * information) but evaluates the finite element field's third derivatives
   * instead of its value.
   *
   * This function may only be used if the finite element in use is a scalar
   * one, i.e. has only one vector component. There is a corresponding
   * function of the same name for vector-valued finite elements.
   *
   * @param[in] fe_function A vector of values that describes (globally) the
   * finite element function that this function should evaluate at the
   * quadrature points of the current cell.
   *
   * @param[out] third_derivatives The third derivatives of the function
   * specified by fe_function at the quadrature points of the current cell.
   * The third derivatives are computed in real space (as opposed to on the
   * unit cell).  The object is assumed to already have the correct size. The
   * data type stored by this output vector must be what you get when you
   * multiply the third derivatives of shape function times the type used to
   * store the values of the unknowns $U_j$ of your finite element vector $U$
   * (represented by the @p fe_function argument).
   *
   * @post <code>third_derivatives[q]</code> will contain the third
   * derivatives of the field described by fe_function at the $q$th quadrature
   * point. <code>third_derivatives[q][i][j][k]</code> represents the
   * $(i,j,k)$th component of the 3rd order tensor of third derivatives at
   * quadrature point $q$.
   *
   * @note The actual data type of the input vector may be either a
   * Vector&lt;T&gt;, BlockVector&lt;T&gt;, or one of the PETSc or Trilinos
   * vector wrapper classes. It represents a global vector of DoF values
   * associated with the DoFHandler object with which this FEValues object was
   * last initialized.
   *
   * @dealiiRequiresUpdateFlags{update_3rd_derivatives}
   */
  template <class InputVector>
  void
  get_function_third_derivatives(
    const InputVector &fe_function,
    std::vector<Tensor<3, spacedim, typename InputVector::value_type>>
      &third_derivatives) const;

  /**
   * This function does the same as the other
   * get_function_third_derivatives(), but applied to multi-component (vector-
   * valued) elements. The meaning of the arguments is as explained there.
   *
   * @post <code>third_derivatives[q]</code> is a vector of third derivatives
   * of the field described by fe_function at the $q$th quadrature point. The
   * size of the vector accessed by <code>third_derivatives[q]</code> equals
   * the number of components of the finite element, i.e.
   * <code>third_derivatives[q][c]</code> returns the third derivative of the
   * $c$th vector component at the $q$th quadrature point. Consequently,
   * <code>third_derivatives[q][c][i][j][k]</code> is the $(i,j,k)$th
   * component of the tensor of third derivatives of the $c$th vector
   * component of the vector field at quadrature point $q$ of the current
   * cell.
   *
   * @dealiiRequiresUpdateFlags{update_3rd_derivatives}
   */
  template <class InputVector>
  void
  get_function_third_derivatives(
    const InputVector &fe_function,
    std::vector<
      std::vector<Tensor<3, spacedim, typename InputVector::value_type>>>
      &        third_derivatives,
    const bool quadrature_points_fastest = false) const;

  /**
   * This function relates to the first of the get_function_third_derivatives()
   * function above in the same way as the get_function_values() with similar
   * arguments relates to the first of the get_function_values() functions. See
   * there for more information.
   *
   * @dealiiRequiresUpdateFlags{update_3rd_derivatives}
   */
  template <class InputVector>
  void
  get_function_third_derivatives(
    const InputVector &                             fe_function,
    const ArrayView<const types::global_dof_index> &indices,
    std::vector<Tensor<3, spacedim, typename InputVector::value_type>>
      &third_derivatives) const;

  /**
   * This function relates to the first of the get_function_third_derivatives()
   * function above in the same way as the get_function_values() with similar
   * arguments relates to the first of the get_function_values() functions. See
   * there for more information.
   *
   * @dealiiRequiresUpdateFlags{update_3rd_derivatives}
   */
  template <class InputVector>
  void
  get_function_third_derivatives(
    const InputVector &                             fe_function,
    const ArrayView<const types::global_dof_index> &indices,
    ArrayView<
      std::vector<Tensor<3, spacedim, typename InputVector::value_type>>>
               third_derivatives,
    const bool quadrature_points_fastest = false) const;
  /** @} */

  /// @name Cell degrees of freedom
  /** @{ */

  /**
   * Return an object that can be thought of as an array containing all
   * indices from zero (inclusive) to `dofs_per_cell` (exclusive). This allows
   * one to write code using range-based `for` loops of the following kind:
   * @code
   *   FEValues<dim>      fe_values (...);
   *   FullMatrix<double> cell_matrix (...);
   *
   *   for (auto &cell : dof_handler.active_cell_iterators())
   *     {
   *       cell_matrix = 0;
   *       fe_values.reinit(cell);
   *       for (const auto q : fe_values.quadrature_point_indices())
   *         for (const auto i : fe_values.dof_indices())
   *           for (const auto j : fe_values.dof_indices())
   *             cell_matrix(i,j) += ...; // Do something for DoF indices (i,j)
   *                                      // at quadrature point q
   *     }
   * @endcode
   * Here, we are looping over all degrees of freedom on all cells, with
   * `i` and `j` taking on all valid indices for cell degrees of freedom, as
   * defined by the finite element passed to `fe_values`.
   */
  std_cxx20::ranges::iota_view<unsigned int, unsigned int>
  dof_indices() const;

  /**
   * Return an object that can be thought of as an array containing all
   * indices from @p start_dof_index (inclusive) to `dofs_per_cell` (exclusive).
   * This allows one to write code using range-based `for` loops of the
   * following kind:
   * @code
   *   FEValues<dim>      fe_values (...);
   *   FullMatrix<double> cell_matrix (...);
   *
   *   for (auto &cell : dof_handler.active_cell_iterators())
   *     {
   *       cell_matrix = 0;
   *       fe_values.reinit(cell);
   *       for (const auto q : fe_values.quadrature_point_indices())
   *         for (const auto i : fe_values.dof_indices())
   *           for (const auto j : fe_values.dof_indices_starting_at(i))
   *             cell_matrix(i,j) += ...; // Do something for DoF indices (i,j)
   *                                      // at quadrature point q
   *     }
   * @endcode
   * Here, we are looping over all local degrees of freedom on all cells, with
   * `i` taking on all valid indices for cell degrees of freedom, as
   * defined by the finite element passed to `fe_values`, and `j` taking
   * on a specified subset of `i`'s range, starting at `i` itself and ending at
   * the number of cell degrees of freedom. In this way, we can construct the
   * upper half and the diagonal of a @ref GlossStiffnessMatrix "stiffness matrix" contribution (assuming it
   * is symmetric, and that only one half of it needs to be computed), for
   * example.
   *
   * @note If the @p start_dof_index is equal to the number of DoFs in the cell,
   * then the returned index range is empty.
   */
  std_cxx20::ranges::iota_view<unsigned int, unsigned int>
  dof_indices_starting_at(const unsigned int start_dof_index) const;

  /**
   * Return an object that can be thought of as an array containing all
   * indices from zero (inclusive) to @p end_dof_index (inclusive). This allows
   * one to write code using range-based `for` loops of the following kind:
   * @code
   *   FEValues<dim>      fe_values (...);
   *   FullMatrix<double> cell_matrix (...);
   *
   *   for (auto &cell : dof_handler.active_cell_iterators())
   *     {
   *       cell_matrix = 0;
   *       fe_values.reinit(cell);
   *       for (const auto q : fe_values.quadrature_point_indices())
   *         for (const auto i : fe_values.dof_indices())
   *           for (const auto j : fe_values.dof_indices_ending_at(i))
   *             cell_matrix(i,j) += ...; // Do something for DoF indices (i,j)
   *                                      // at quadrature point q
   *     }
   * @endcode
   * Here, we are looping over all local degrees of freedom on all cells, with
   * `i` taking on all valid indices for cell degrees of freedom, as
   * defined by the finite element passed to `fe_values`, and `j` taking
   * on a specified subset of `i`'s range, starting at zero and ending at
   * `i` itself. In this way, we can construct the lower half and the
   * diagonal of a @ref GlossStiffnessMatrix "stiffness matrix" contribution (assuming it is symmetric, and
   * that only one half of it needs to be computed), for example.
   *
   * @note If the @p end_dof_index is equal to zero, then the returned index
   * range is empty.
   */
  std_cxx20::ranges::iota_view<unsigned int, unsigned int>
  dof_indices_ending_at(const unsigned int end_dof_index) const;

  /** @} */

  /// @name Geometry of the cell
  /** @{ */

  /**
   * Return an object that can be thought of as an array containing all
   * indices from zero to `n_quadrature_points`. This allows to write code
   * using range-based `for` loops of the following kind:
   * @code
   *   FEValues<dim> fe_values (...);
   *
   *   for (auto &cell : dof_handler.active_cell_iterators())
   *     {
   *       fe_values.reinit(cell);
   *       for (const auto q_point : fe_values.quadrature_point_indices())
   *         ... do something at the quadrature point ...
   *     }
   * @endcode
   * Here, we are looping over all quadrature points on all cells, with
   * `q_point` taking on all valid indices for quadrature points, as defined
   * by the quadrature rule passed to `fe_values`.
   *
   * @see CPP11
   */
  std_cxx20::ranges::iota_view<unsigned int, unsigned int>
  quadrature_point_indices() const;

  /**
   * Position of the <tt>q</tt>th quadrature point in real space.
   *
   * @dealiiRequiresUpdateFlags{update_quadrature_points}
   */
  const Point<spacedim> &
  quadrature_point(const unsigned int q) const;

  /**
   * Return a reference to the vector of quadrature points in real space.
   *
   * @dealiiRequiresUpdateFlags{update_quadrature_points}
   */
  const std::vector<Point<spacedim>> &
  get_quadrature_points() const;

  /**
   * Mapped quadrature weight. If this object refers to a volume evaluation
   * (i.e. the derived class is of type FEValues), then this is the Jacobi
   * determinant times the weight of the *<tt>i</tt>th unit quadrature point.
   *
   * For surface evaluations (i.e. classes FEFaceValues or FESubfaceValues),
   * it is the mapped surface element times the weight of the quadrature
   * point.
   *
   * You can think of the quantity returned by this function as the volume or
   * surface element $dx, ds$ in the integral that we implement here by
   * quadrature.
   *
   * @dealiiRequiresUpdateFlags{update_JxW_values}
   */
  double
  JxW(const unsigned int quadrature_point) const;

  /**
   * Return a reference to the array holding the values returned by JxW().
   */
  const std::vector<double> &
  get_JxW_values() const;

  /**
   * Return the Jacobian of the transformation at the specified quadrature
   * point, i.e.  $J_{ij}=dx_i/d\hat x_j$
   *
   * @dealiiRequiresUpdateFlags{update_jacobians}
   */
  const DerivativeForm<1, dim, spacedim> &
  jacobian(const unsigned int quadrature_point) const;

  /**
   * Return a reference to the array holding the values returned by
   * jacobian().
   *
   * @dealiiRequiresUpdateFlags{update_jacobians}
   */
  const std::vector<DerivativeForm<1, dim, spacedim>> &
  get_jacobians() const;

  /**
   * Return the second derivative of the transformation from unit to real
   * cell, i.e. the first derivative of the Jacobian, at the specified
   * quadrature point, i.e. $G_{ijk}=dJ_{jk}/d\hat x_i$.
   *
   * @dealiiRequiresUpdateFlags{update_jacobian_grads}
   */
  const DerivativeForm<2, dim, spacedim> &
  jacobian_grad(const unsigned int quadrature_point) const;

  /**
   * Return a reference to the array holding the values returned by
   * jacobian_grads().
   *
   * @dealiiRequiresUpdateFlags{update_jacobian_grads}
   */
  const std::vector<DerivativeForm<2, dim, spacedim>> &
  get_jacobian_grads() const;

  /**
   * Return the second derivative of the transformation from unit to real
   * cell, i.e. the first derivative of the Jacobian, at the specified
   * quadrature point, pushed forward to the real cell coordinates, i.e.
   * $G_{ijk}=dJ_{iJ}/d\hat x_K (J_{jJ})^{-1} (J_{kK})^{-1}$.
   *
   * @dealiiRequiresUpdateFlags{update_jacobian_pushed_forward_grads}
   */
  const Tensor<3, spacedim> &
  jacobian_pushed_forward_grad(const unsigned int quadrature_point) const;

  /**
   * Return a reference to the array holding the values returned by
   * jacobian_pushed_forward_grads().
   *
   * @dealiiRequiresUpdateFlags{update_jacobian_pushed_forward_grads}
   */
  const std::vector<Tensor<3, spacedim>> &
  get_jacobian_pushed_forward_grads() const;

  /**
   * Return the third derivative of the transformation from unit to real cell,
   * i.e. the second derivative of the Jacobian, at the specified quadrature
   * point, i.e. $G_{ijkl}=\frac{d^2J_{ij}}{d\hat x_k d\hat x_l}$.
   *
   * @dealiiRequiresUpdateFlags{update_jacobian_2nd_derivatives}
   */
  const DerivativeForm<3, dim, spacedim> &
  jacobian_2nd_derivative(const unsigned int quadrature_point) const;

  /**
   * Return a reference to the array holding the values returned by
   * jacobian_2nd_derivatives().
   *
   * @dealiiRequiresUpdateFlags{update_jacobian_2nd_derivatives}
   */
  const std::vector<DerivativeForm<3, dim, spacedim>> &
  get_jacobian_2nd_derivatives() const;

  /**
   * Return the third derivative of the transformation from unit to real cell,
   * i.e. the second derivative of the Jacobian, at the specified quadrature
   * point, pushed forward to the real cell coordinates, i.e.
   * $G_{ijkl}=\frac{d^2J_{iJ}}{d\hat x_K d\hat x_L} (J_{jJ})^{-1}
   * (J_{kK})^{-1}(J_{lL})^{-1}$.
   *
   * @dealiiRequiresUpdateFlags{update_jacobian_pushed_forward_2nd_derivatives}
   */
  const Tensor<4, spacedim> &
  jacobian_pushed_forward_2nd_derivative(
    const unsigned int quadrature_point) const;

  /**
   * Return a reference to the array holding the values returned by
   * jacobian_pushed_forward_2nd_derivatives().
   *
   * @dealiiRequiresUpdateFlags{update_jacobian_pushed_forward_2nd_derivatives}
   */
  const std::vector<Tensor<4, spacedim>> &
  get_jacobian_pushed_forward_2nd_derivatives() const;

  /**
   * Return the fourth derivative of the transformation from unit to real
   * cell, i.e. the third derivative of the Jacobian, at the specified
   * quadrature point, i.e. $G_{ijklm}=\frac{d^2J_{ij}}{d\hat x_k d\hat x_l
   * d\hat x_m}$.
   *
   * @dealiiRequiresUpdateFlags{update_jacobian_3rd_derivatives}
   */
  const DerivativeForm<4, dim, spacedim> &
  jacobian_3rd_derivative(const unsigned int quadrature_point) const;

  /**
   * Return a reference to the array holding the values returned by
   * jacobian_3rd_derivatives().
   *
   * @dealiiRequiresUpdateFlags{update_jacobian_3rd_derivatives}
   */
  const std::vector<DerivativeForm<4, dim, spacedim>> &
  get_jacobian_3rd_derivatives() const;

  /**
   * Return the fourth derivative of the transformation from unit to real
   * cell, i.e. the third derivative of the Jacobian, at the specified
   * quadrature point, pushed forward to the real cell coordinates, i.e.
   * $G_{ijklm}=\frac{d^3J_{iJ}}{d\hat x_K d\hat x_L d\hat x_M} (J_{jJ})^{-1}
   * (J_{kK})^{-1} (J_{lL})^{-1} (J_{mM})^{-1}$.
   *
   * @dealiiRequiresUpdateFlags{update_jacobian_pushed_forward_3rd_derivatives}
   */
  const Tensor<5, spacedim> &
  jacobian_pushed_forward_3rd_derivative(
    const unsigned int quadrature_point) const;

  /**
   * Return a reference to the array holding the values returned by
   * jacobian_pushed_forward_3rd_derivatives().
   *
   * @dealiiRequiresUpdateFlags{update_jacobian_pushed_forward_2nd_derivatives}
   */
  const std::vector<Tensor<5, spacedim>> &
  get_jacobian_pushed_forward_3rd_derivatives() const;

  /**
   * Return the inverse Jacobian of the transformation at the specified
   * quadrature point, i.e.  $J_{ij}=d\hat x_i/dx_j$
   *
   * @dealiiRequiresUpdateFlags{update_inverse_jacobians}
   */
  const DerivativeForm<1, spacedim, dim> &
  inverse_jacobian(const unsigned int quadrature_point) const;

  /**
   * Return a reference to the array holding the values returned by
   * inverse_jacobian().
   *
   * @dealiiRequiresUpdateFlags{update_inverse_jacobians}
   */
  const std::vector<DerivativeForm<1, spacedim, dim>> &
  get_inverse_jacobians() const;

  /**
   * Return the normal vector at a quadrature point. If you call this
   * function for a face (i.e., when using a FEFaceValues or FESubfaceValues
   * object), then this function returns the outward normal vector to
   * the cell at the <tt>i</tt>th quadrature point of the face.
   *
   * In contrast, if you call this function for a cell of codimension one
   * (i.e., when using a `FEValues<dim,spacedim>` object with
   * `spacedim>dim`), then this function returns the normal vector to the
   * cell -- in other words, an approximation to the normal vector to the
   * manifold in which the triangulation is embedded. There are of
   * course two normal directions to a manifold in that case, and this
   * function returns the "up" direction as induced by the numbering of the
   * vertices.
   *
   * The length of the vector is normalized to one.
   *
   * @dealiiRequiresUpdateFlags{update_normal_vectors}
   */
  const Tensor<1, spacedim> &
  normal_vector(const unsigned int i) const;

  /**
   * Return the normal vectors at all quadrature points represented by
   * this object. See the normal_vector() function for what the normal
   * vectors represent.
   *
   * @dealiiRequiresUpdateFlags{update_normal_vectors}
   */
  const std::vector<Tensor<1, spacedim>> &
  get_normal_vectors() const;

  /** @} */

  /// @name Extractors Methods to extract individual components
  /** @{ */

  /**
   * Create a view of the current FEValues object that represents a particular
   * scalar component of the possibly vector-valued finite element. The
   * concept of views is explained in the documentation of the namespace
   * FEValuesViews and in particular in the
   * @ref vector_valued
   * module.
   */
  const FEValuesViews::Scalar<dim, spacedim> &
  operator[](const FEValuesExtractors::Scalar &scalar) const;

  /**
   * Create a view of the current FEValues object that represents a set of
   * <code>dim</code> scalar components (i.e. a vector) of the vector-valued
   * finite element. The concept of views is explained in the documentation of
   * the namespace FEValuesViews and in particular in the
   * @ref vector_valued
   * module.
   */
  const FEValuesViews::Vector<dim, spacedim> &
  operator[](const FEValuesExtractors::Vector &vector) const;

  /**
   * Create a view of the current FEValues object that represents a set of
   * <code>(dim*dim + dim)/2</code> scalar components (i.e. a symmetric 2nd
   * order tensor) of the vector-valued finite element. The concept of views
   * is explained in the documentation of the namespace FEValuesViews and in
   * particular in the
   * @ref vector_valued
   * module.
   */
  const FEValuesViews::SymmetricTensor<2, dim, spacedim> &
  operator[](const FEValuesExtractors::SymmetricTensor<2> &tensor) const;


  /**
   * Create a view of the current FEValues object that represents a set of
   * <code>(dim*dim)</code> scalar components (i.e. a 2nd order tensor) of the
   * vector-valued finite element. The concept of views is explained in the
   * documentation of the namespace FEValuesViews and in particular in the
   * @ref vector_valued
   * module.
   */
  const FEValuesViews::Tensor<2, dim, spacedim> &
  operator[](const FEValuesExtractors::Tensor<2> &tensor) const;

  /** @} */

  /// @name Access to the raw data
  /** @{ */

  /**
   * Constant reference to the selected mapping object.
   */
  const Mapping<dim, spacedim> &
  get_mapping() const;

  /**
   * Constant reference to the selected finite element object.
   */
  const FiniteElement<dim, spacedim> &
  get_fe() const;

  /**
   * Return the update flags set for this object.
   */
  UpdateFlags
  get_update_flags() const;

  /**
   * Return a triangulation iterator to the current cell.
   */
  const typename Triangulation<dim, spacedim>::cell_iterator
  get_cell() const;

  /**
   * Return the relation of the current cell to the previous cell. This allows
   * re-use of some cell data (like local matrices for equations with constant
   * coefficients) if the result is <tt>CellSimilarity::translation</tt>.
   */
  CellSimilarity::Similarity
  get_cell_similarity() const;

  /**
   * Determine an estimate for the memory consumption (in bytes) of this
   * object.
   */
  std::size_t
  memory_consumption() const;
  /** @} */


  /**
   * This exception is thrown if FEValuesBase is asked to return the value of
   * a field which was not required by the UpdateFlags for this FEValuesBase.
   *
   * @ingroup Exceptions
   */
  DeclException1(
    ExcAccessToUninitializedField,
    std::string,
    << "You are requesting information from an FEValues/FEFaceValues/FESubfaceValues "
    << "object for which this kind of information has not been computed. What "
    << "information these objects compute is determined by the update_* flags you "
    << "pass to the constructor. Here, the operation you are attempting requires "
    << "the <" << arg1
    << "> flag to be set, but it was apparently not specified "
    << "upon construction.");

  /**
   * FEValues::reinit() has not been called for any cell.
   *
   * @ingroup Exceptions
   */
  DeclExceptionMsg(ExcNotReinited,
                   "FEValues object is not reinit'ed to any cell");

  /**
   * Mismatch between the FEValues FiniteElement and
   * cell->get_dof_handler().get_fe()
   *
   * @ingroup Exceptions
   */
  DeclExceptionMsg(
    ExcFEDontMatch,
    "The FiniteElement you provided to FEValues and the FiniteElement that belongs "
    "to the DoFHandler that provided the cell iterator do not match.");
  /**
   * A given shape function is not primitive, but it needs to be.
   *
   * @ingroup Exceptions
   */
  DeclException1(ExcShapeFunctionNotPrimitive,
                 int,
                 << "The shape function with index " << arg1
                 << " is not primitive, i.e. it is vector-valued and "
                 << "has more than one non-zero vector component. This "
                 << "function cannot be called for these shape functions. "
                 << "Maybe you want to use the same function with the "
                 << "_component suffix?");

  /**
   * The given FiniteElement is not a primitive element, see
   * FiniteElement::is_primitive().
   *
   * @ingroup Exceptions
   */
  DeclExceptionMsg(
    ExcFENotPrimitive,
    "The given FiniteElement is not a primitive element but the requested operation "
    "only works for those. See FiniteElement::is_primitive() for more information.");

protected:
  /**
   * Objects of the FEValues class need to store an iterator
   * to the present cell in order to be able to extract the values of the
   * degrees of freedom on this cell in the get_function_values() and assorted
   * functions.
   */
  class CellIteratorContainer
  {
  public:
    DeclExceptionMsg(
      ExcNeedsDoFHandler,
      "You have previously called the FEValues::reinit() function with a "
      "cell iterator of type Triangulation<dim,spacedim>::cell_iterator. However, "
      "when you do this, you cannot call some functions in the FEValues "
      "class, such as the get_function_values/gradients/hessians/third_derivatives "
      "functions. If you need these functions, then you need to call "
      "FEValues::reinit() with an iterator type that allows to extract "
      "degrees of freedom, such as DoFHandler<dim,spacedim>::cell_iterator.");

    /**
     * Constructor.
     */
    CellIteratorContainer();

    /**
     * Constructor.
     */
    template <bool lda>
    CellIteratorContainer(
      const TriaIterator<DoFCellAccessor<dim, spacedim, lda>> &cell);

    /**
     * Constructor.
     */
    CellIteratorContainer(
      const typename Triangulation<dim, spacedim>::cell_iterator &cell);

    /**
     * Indicate whether FEValues::reinit() was called.
     */
    bool
    is_initialized() const;

    /**
     * Conversion operator to an iterator for triangulations. This
     * conversion is implicit for the original iterators, since they are derived
     * classes. However, since here we have kind of a parallel class hierarchy,
     * we have to have a conversion operator.
     */
    operator typename Triangulation<dim, spacedim>::cell_iterator() const;

    /**
     * Return the number of degrees of freedom the DoF
     * handler object has to which the iterator belongs to.
     */
    types::global_dof_index
    n_dofs_for_dof_handler() const;

    /**
     * Call @p get_interpolated_dof_values of the iterator with the
     * given arguments.
     */
    template <typename VectorType>
    void
    get_interpolated_dof_values(
      const VectorType &                       in,
      Vector<typename VectorType::value_type> &out) const;

    /**
     * Call @p get_interpolated_dof_values of the iterator with the
     * given arguments.
     */
    void
    get_interpolated_dof_values(const IndexSet &              in,
                                Vector<IndexSet::value_type> &out) const;

  private:
    bool                                                 initialized;
    typename Triangulation<dim, spacedim>::cell_iterator cell;
    const DoFHandler<dim, spacedim> *                    dof_handler;
    bool                                                 level_dof_access;
  };

  /**
   * Store the cell selected last time the reinit() function was called.  This
   * is necessary for the <tt>get_function_*</tt> functions as well as the
   * functions of same name in the extractor classes.
   */
  CellIteratorContainer present_cell;

  /**
   * A signal connection we use to ensure we get informed whenever the
   * triangulation changes by refinement. We need to know about that because
   * it invalidates all cell iterators and, as part of that, the
   * 'present_cell' iterator we keep around between subsequent calls to
   * reinit() in order to compute the cell similarity.
   */
  boost::signals2::connection tria_listener_refinement;

  /**
   * A signal connection we use to ensure we get informed whenever the
   * triangulation changes by mesh transformations. We need to know about that
   * because it invalidates all cell iterators and, as part of that, the
   * 'present_cell' iterator we keep around between subsequent calls to
   * reinit() in order to compute the cell similarity.
   */
  boost::signals2::connection tria_listener_mesh_transform;

  /**
   * A function that is connected to the triangulation in order to reset the
   * stored 'present_cell' iterator to an invalid one whenever the
   * triangulation is changed and the iterator consequently becomes invalid.
   */
  void
  invalidate_present_cell();

  /**
   * This function is called by the various reinit() functions in derived
   * classes. Given the cell indicated by the argument, test whether we have
   * to throw away the previously stored present_cell argument because it
   * would require us to compare cells from different triangulations. In
   * checking all this, also make sure that we have tria_listener connected to
   * the triangulation to which we will set present_cell right after calling
   * this function.
   */
  void
  maybe_invalidate_previous_present_cell(
    const typename Triangulation<dim, spacedim>::cell_iterator &cell);

  /**
   * A pointer to the mapping object associated with this FEValues object.
   */
  const SmartPointer<const Mapping<dim, spacedim>, FEValuesBase<dim, spacedim>>
    mapping;

  /**
   * A pointer to the internal data object of mapping, obtained from
   * Mapping::get_data(), Mapping::get_face_data(), or
   * Mapping::get_subface_data().
   */
  std::unique_ptr<typename Mapping<dim, spacedim>::InternalDataBase>
    mapping_data;

  /**
   * An object into which the Mapping::fill_fe_values() and similar functions
   * place their output.
   */
  internal::FEValuesImplementation::MappingRelatedData<dim, spacedim>
    mapping_output;


  /**
   * A pointer to the finite element object associated with this FEValues
   * object.
   */
  const SmartPointer<const FiniteElement<dim, spacedim>,
                     FEValuesBase<dim, spacedim>>
    fe;

  /**
   * A pointer to the internal data object of finite element, obtained from
   * FiniteElement::get_data(), Mapping::get_face_data(), or
   * FiniteElement::get_subface_data().
   */
  std::unique_ptr<typename FiniteElement<dim, spacedim>::InternalDataBase>
    fe_data;

  /**
   * An object into which the FiniteElement::fill_fe_values() and similar
   * functions place their output.
   */
  dealii::internal::FEValuesImplementation::FiniteElementRelatedData<dim,
                                                                     spacedim>
    finite_element_output;


  /**
   * Original update flags handed to the constructor of FEValues.
   */
  UpdateFlags update_flags;

  /**
   * Initialize some update flags. Called from the @p initialize functions of
   * derived classes, which are in turn called from their constructors.
   *
   * Basically, this function finds out using the finite element and mapping
   * object already stored which flags need to be set to compute everything
   * the user wants, as expressed through the flags passed as argument.
   */
  UpdateFlags
  compute_update_flags(const UpdateFlags update_flags) const;

  /**
   * An enum variable that can store different states of the current cell in
   * comparison to the previously visited cell. If wanted, additional states
   * can be checked here and used in one of the methods used during reinit.
   */
  CellSimilarity::Similarity cell_similarity;

  /**
   * A function that checks whether the new cell is similar to the one
   * previously used. Then, a significant amount of the data can be reused,
   * e.g. the derivatives of the basis functions in real space, shape_grad.
   */
  void
  check_cell_similarity(
    const typename Triangulation<dim, spacedim>::cell_iterator &cell);

private:
  /**
   * A cache for all possible FEValuesViews objects.
   */
  dealii::internal::FEValuesViews::Cache<dim, spacedim> fe_values_views_cache;

  // Make the view classes friends of this class, since they access internal
  // data.
  template <int, int>
  friend class FEValuesViews::Scalar;
  template <int, int>
  friend class FEValuesViews::Vector;
  template <int, int, int>
  friend class FEValuesViews::SymmetricTensor;
  template <int, int, int>
  friend class FEValuesViews::Tensor;
};



/**
 * Finite element evaluated in quadrature points of a cell.
 *
 * This function implements the initialization routines for FEValuesBase, if
 * values in quadrature points of a cell are needed. For further documentation
 * see this class.
 *
 * @ingroup feaccess
 */
template <int dim, int spacedim = dim>
class FEValues : public FEValuesBase<dim, spacedim>
{
public:
  /**
   * Dimension of the object over which we integrate. For the present class,
   * this is equal to <code>dim</code>.
   */
  static constexpr unsigned int integral_dimension = dim;

  /**
   * Constructor. Gets cell independent data from mapping and finite element
   * objects, matching the quadrature rule and update flags.
   */
  FEValues(const Mapping<dim, spacedim> &      mapping,
           const FiniteElement<dim, spacedim> &fe,
           const Quadrature<dim> &             quadrature,
           const UpdateFlags                   update_flags);

  /**
   * Like the function above, but taking a collection of quadrature rules.
   *
   * @note We require, in contrast to FEFaceValues, that the number of quadrature
   *   rules in the collection is one.
   */
  FEValues(const Mapping<dim, spacedim> &      mapping,
           const FiniteElement<dim, spacedim> &fe,
           const hp::QCollection<dim> &        quadrature,
           const UpdateFlags                   update_flags);

  /**
   * Constructor. This constructor is equivalent to the other one except that
   * it makes the object use a $Q_1$ mapping (i.e., an object of type
   * MappingQ(1)) implicitly.
   */
  FEValues(const FiniteElement<dim, spacedim> &fe,
           const Quadrature<dim> &             quadrature,
           const UpdateFlags                   update_flags);

  /**
   * Like the function above, but taking a collection of quadrature rules.
   *
   * @note We require, in contrast to FEFaceValues, that the number of quadrature
   *   rules in the collection is one.
   */
  FEValues(const FiniteElement<dim, spacedim> &fe,
           const hp::QCollection<dim> &        quadrature,
           const UpdateFlags                   update_flags);

  /**
   * Reinitialize the gradients, Jacobi determinants, etc for the given cell
   * of type "iterator into a DoFHandler object", and the finite element
   * associated with this object. It is assumed that the finite element used
   * by the given cell is also the one used by this FEValues object.
   */
  template <bool level_dof_access>
  void
  reinit(
    const TriaIterator<DoFCellAccessor<dim, spacedim, level_dof_access>> &cell);

  /**
   * Reinitialize the gradients, Jacobi determinants, etc for the given cell
   * of type "iterator into a Triangulation object", and the given finite
   * element. Since iterators into triangulation alone only convey information
   * about the geometry of a cell, but not about degrees of freedom possibly
   * associated with this cell, you will not be able to call some functions of
   * this class if they need information about degrees of freedom. These
   * functions are, above all, the
   * <tt>get_function_value/gradients/hessians/laplacians/third_derivatives</tt>
   * functions. If you want to call these functions, you have to call the @p
   * reinit variants that take iterators into DoFHandler or other DoF handler
   * type objects.
   */
  void
  reinit(const typename Triangulation<dim, spacedim>::cell_iterator &cell);

  /**
   * Return a reference to the copy of the quadrature formula stored by this
   * object.
   */
  const Quadrature<dim> &
  get_quadrature() const;

  /**
   * Determine an estimate for the memory consumption (in bytes) of this
   * object.
   */
  std::size_t
  memory_consumption() const;

  /**
   * Return a reference to this very object.
   *
   * Though it seems that it is not very useful, this function is there to
   * provide capability to the hp::FEValues class, in which case it provides
   * the FEValues object for the present cell (remember that for hp-finite
   * elements, the actual FE object used may change from cell to cell, so we
   * also need different FEValues objects for different cells; once you
   * reinitialize the hp::FEValues object for a specific cell, it retrieves
   * the FEValues object for the FE on that cell and returns it through a
   * function of the same name as this one; this function here therefore only
   * provides the same interface so that one can templatize on FEValues and
   * hp::FEValues).
   */
  const FEValues<dim, spacedim> &
  get_present_fe_values() const;

private:
  /**
   * Store a copy of the quadrature formula here.
   */
  const Quadrature<dim> quadrature;

  /**
   * Do work common to the two constructors.
   */
  void
  initialize(const UpdateFlags update_flags);

  /**
   * The reinit() functions do only that part of the work that requires
   * knowledge of the type of iterator. After setting present_cell(), they
   * pass on to this function, which does the real work, and which is
   * independent of the actual type of the cell iterator.
   */
  void
  do_reinit();
};


/**
 * Extend the interface of FEValuesBase to values that only make sense when
 * evaluating something on the surface of a cell. All the data that is
 * available in the interior of cells is also available here.
 *
 * See FEValuesBase
 *
 * @ingroup feaccess
 */
template <int dim, int spacedim = dim>
class FEFaceValuesBase : public FEValuesBase<dim, spacedim>
{
public:
  /**
   * Dimension of the object over which we integrate. For the present class,
   * this is equal to <code>dim-1</code>.
   */
  static constexpr unsigned int integral_dimension = dim - 1;

  /**
   * Constructor. Call the constructor of the base class and set up the arrays
   * of this class with the right sizes.  Actually filling these arrays is a
   * duty of the derived class's constructors.
   *
   * @p n_faces_or_subfaces is the number of faces or subfaces that this
   * object is to store. The actual number depends on the derived class, for
   * FEFaceValues it is <tt>2*dim</tt>, while for the FESubfaceValues class it
   * is <tt>2*dim*(1<<(dim-1))</tt>, i.e. the number of faces times the number
   * of subfaces per face.
   */
  FEFaceValuesBase(const unsigned int                  dofs_per_cell,
                   const UpdateFlags                   update_flags,
                   const Mapping<dim, spacedim> &      mapping,
                   const FiniteElement<dim, spacedim> &fe,
                   const Quadrature<dim - 1> &         quadrature);

  /**
   * Like the function above, but taking a collection of quadrature rules. This
   * allows to assign each face a different quadrature rule. In the case that
   * the collection only contains a single face quadrature, this quadrature
   * rule is use on all faces.
   */
  FEFaceValuesBase(const unsigned int                  dofs_per_cell,
                   const UpdateFlags                   update_flags,
                   const Mapping<dim, spacedim> &      mapping,
                   const FiniteElement<dim, spacedim> &fe,
                   const hp::QCollection<dim - 1> &    quadrature);

  /**
   * Boundary form of the transformation of the cell at the <tt>i</tt>th
   * quadrature point.  See
   * @ref GlossBoundaryForm.
   *
   * @dealiiRequiresUpdateFlags{update_boundary_forms}
   */
  const Tensor<1, spacedim> &
  boundary_form(const unsigned int i) const;

  /**
   * Return the list of outward normal vectors times the Jacobian of the
   * surface mapping.
   *
   * @dealiiRequiresUpdateFlags{update_boundary_forms}
   */
  const std::vector<Tensor<1, spacedim>> &
  get_boundary_forms() const;

  /**
   * Return the number of the face selected the last time the reinit() function
   * was called.
   */
  unsigned int
  get_face_number() const;

  /**
   * Return the index of the face selected the last time the reinit() function
   * was called.
   */
  unsigned int
  get_face_index() const;

  /**
   * Return a reference to the copy of the quadrature formula stored by this
   * object.
   */
  const Quadrature<dim - 1> &
  get_quadrature() const;

  /**
   * Determine an estimate for the memory consumption (in bytes) of this
   * object.
   */
  std::size_t
  memory_consumption() const;

protected:
  /**
   * Number of the face selected the last time the reinit() function was
   * called.
   */
  unsigned int present_face_no;

  /**
   * Index of the face selected the last time the reinit() function was
   * called.
   */
  unsigned int present_face_index;

  /**
   * Store a copy of the quadrature formula here.
   */
  const hp::QCollection<dim - 1> quadrature;
};



/**
 * Finite element evaluated in quadrature points on a face.
 *
 * This class adds the functionality of FEFaceValuesBase to FEValues; see
 * there for more documentation.
 *
 * Since finite element functions and their derivatives may be discontinuous
 * at cell boundaries, there is no restriction of this function to a mesh
 * face. But, there are limits of these values approaching the face from
 * either of the neighboring cells.
 *
 * @ingroup feaccess
 */
template <int dim, int spacedim = dim>
class FEFaceValues : public FEFaceValuesBase<dim, spacedim>
{
public:
  /**
   * Dimension in which this object operates.
   */

  static constexpr unsigned int dimension = dim;

  static constexpr unsigned int space_dimension = spacedim;

  /**
   * Dimension of the object over which we integrate. For the present class,
   * this is equal to <code>dim-1</code>.
   */
  static constexpr unsigned int integral_dimension = dim - 1;

  /**
   * Constructor. Gets cell independent data from mapping and finite element
   * objects, matching the quadrature rule and update flags.
   */
  FEFaceValues(const Mapping<dim, spacedim> &      mapping,
               const FiniteElement<dim, spacedim> &fe,
               const Quadrature<dim - 1> &         quadrature,
               const UpdateFlags                   update_flags);

  /**
   * Like the function above, but taking a collection of quadrature rules. This
   * allows to assign each face a different quadrature rule. In the case that
   * the collection only contains a single face quadrature, this quadrature
   * rule is use on all faces.
   */
  FEFaceValues(const Mapping<dim, spacedim> &      mapping,
               const FiniteElement<dim, spacedim> &fe,
               const hp::QCollection<dim - 1> &    quadrature,
               const UpdateFlags                   update_flags);

  /**
   * Constructor. This constructor is equivalent to the other one except that
   * it makes the object use a $Q_1$ mapping (i.e., an object of type
   * MappingQ(1)) implicitly.
   */
  FEFaceValues(const FiniteElement<dim, spacedim> &fe,
               const Quadrature<dim - 1> &         quadrature,
               const UpdateFlags                   update_flags);

  /**
   * Like the function above, but taking a collection of quadrature rules. This
   * allows to assign each face a different quadrature rule. In the case that
   * the collection only contains a single face quadrature, this quadrature
   * rule is use on all faces.
   */
  FEFaceValues(const FiniteElement<dim, spacedim> &fe,
               const hp::QCollection<dim - 1> &    quadrature,
               const UpdateFlags                   update_flags);

  /**
   * Reinitialize the gradients, Jacobi determinants, etc for the face with
   * number @p face_no of @p cell and the given finite element.
   */
  template <bool level_dof_access>
  void
  reinit(
    const TriaIterator<DoFCellAccessor<dim, spacedim, level_dof_access>> &cell,
    const unsigned int face_no);

  /**
   * Reinitialize the gradients, Jacobi determinants, etc for face @p face
   * and cell @p cell.
   *
   * @note @p face must be one of @p cell's face iterators.
   */
  template <bool level_dof_access>
  void
  reinit(
    const TriaIterator<DoFCellAccessor<dim, spacedim, level_dof_access>> &cell,
    const typename Triangulation<dim, spacedim>::face_iterator &          face);

  /**
   * Reinitialize the gradients, Jacobi determinants, etc for the given face
   * on a given cell of type "iterator into a Triangulation object", and the
   * given finite element. Since iterators into a triangulation alone only
   * convey information about the geometry of a cell, but not about degrees of
   * freedom possibly associated with this cell, you will not be able to call
   * some functions of this class if they need information about degrees of
   * freedom. These functions are, above all, the
   * <tt>get_function_value/gradients/hessians/third_derivatives</tt>
   * functions. If you want to call these functions, you have to call the @p
   * reinit variants that take iterators into DoFHandler or other DoF handler
   * type objects.
   */
  void
  reinit(const typename Triangulation<dim, spacedim>::cell_iterator &cell,
         const unsigned int                                          face_no);

  /*
   * Reinitialize the gradients, Jacobi determinants, etc for the given face
   * on a given cell of type "iterator into a Triangulation object", and the
   * given finite element. Since iterators into a triangulation alone only
   * convey information about the geometry of a cell, but not about degrees of
   * freedom possibly associated with this cell, you will not be able to call
   * some functions of this class if they need information about degrees of
   * freedom. These functions are, above all, the
   * <tt>get_function_value/gradients/hessians/third_derivatives</tt>
   * functions. If you want to call these functions, you have to call the @p
   * reinit variants that take iterators into DoFHandler or other DoF handler
   * type objects.
   *
   * @note @p face must be one of @p cell's face iterators.
   */
  void
  reinit(const typename Triangulation<dim, spacedim>::cell_iterator &cell,
         const typename Triangulation<dim, spacedim>::face_iterator &face);

  /**
   * Return a reference to this very object.
   *
   * Though it seems that it is not very useful, this function is there to
   * provide capability to the hp::FEValues class, in which case it provides
   * the FEValues object for the present cell (remember that for hp-finite
   * elements, the actual FE object used may change from cell to cell, so we
   * also need different FEValues objects for different cells; once you
   * reinitialize the hp::FEValues object for a specific cell, it retrieves
   * the FEValues object for the FE on that cell and returns it through a
   * function of the same name as this one; this function here therefore only
   * provides the same interface so that one can templatize on FEValues and
   * hp::FEValues).
   */
  const FEFaceValues<dim, spacedim> &
  get_present_fe_values() const;

private:
  /**
   * Do work common to the two constructors.
   */
  void
  initialize(const UpdateFlags update_flags);

  /**
   * The reinit() functions do only that part of the work that requires
   * knowledge of the type of iterator. After setting present_cell(), they
   * pass on to this function, which does the real work, and which is
   * independent of the actual type of the cell iterator.
   */
  void
  do_reinit(const unsigned int face_no);
};


/**
 * Finite element evaluated in quadrature points on a face.
 *
 * This class adds the functionality of FEFaceValuesBase to FEValues; see
 * there for more documentation.
 *
 * This class is used for faces lying on a refinement edge. In this case, the
 * neighboring cell is refined. To be able to compute differences between
 * interior and exterior function values, the refinement of the neighboring
 * cell must be simulated on this cell. This is achieved by applying a
 * quadrature rule that simulates the refinement. The resulting data fields
 * are split up to reflect the refinement structure of the neighbor: a subface
 * number corresponds to the number of the child of the neighboring face.
 *
 * @ingroup feaccess
 */
template <int dim, int spacedim = dim>
class FESubfaceValues : public FEFaceValuesBase<dim, spacedim>
{
public:
  /**
   * Dimension in which this object operates.
   */
  static constexpr unsigned int dimension = dim;

  /**
   * Dimension of the space in which this object operates.
   */
  static constexpr unsigned int space_dimension = spacedim;

  /**
   * Dimension of the object over which we integrate. For the present class,
   * this is equal to <code>dim-1</code>.
   */
  static constexpr unsigned int integral_dimension = dim - 1;

  /**
   * Constructor. Gets cell independent data from mapping and finite element
   * objects, matching the quadrature rule and update flags.
   */
  FESubfaceValues(const Mapping<dim, spacedim> &      mapping,
                  const FiniteElement<dim, spacedim> &fe,
                  const Quadrature<dim - 1> &         face_quadrature,
                  const UpdateFlags                   update_flags);

  /**
   * Like the function above, but taking a collection of quadrature rules.
   *
   * @note We require, in contrast to FEFaceValues, that the number of quadrature
   *   rules in the collection is one.
   */
  FESubfaceValues(const Mapping<dim, spacedim> &      mapping,
                  const FiniteElement<dim, spacedim> &fe,
                  const hp::QCollection<dim - 1> &    face_quadrature,
                  const UpdateFlags                   update_flags);

  /**
   * Constructor. This constructor is equivalent to the other one except that
   * it makes the object use a $Q_1$ mapping (i.e., an object of type
   * MappingQ(1)) implicitly.
   */
  FESubfaceValues(const FiniteElement<dim, spacedim> &fe,
                  const Quadrature<dim - 1> &         face_quadrature,
                  const UpdateFlags                   update_flags);

  /**
   * Like the function above, but taking a collection of quadrature rules.
   *
   * @note We require, in contrast to FEFaceValues, that the number of quadrature
   *   rules in the collection is one.
   */
  FESubfaceValues(const FiniteElement<dim, spacedim> &fe,
                  const hp::QCollection<dim - 1> &    face_quadrature,
                  const UpdateFlags                   update_flags);

  /**
   * Reinitialize the gradients, Jacobi determinants, etc for the given cell
   * of type "iterator into a DoFHandler object", and the finite element
   * associated with this object. It is assumed that the finite element used
   * by the given cell is also the one used by this FESubfaceValues object.
   */
  template <bool level_dof_access>
  void
  reinit(
    const TriaIterator<DoFCellAccessor<dim, spacedim, level_dof_access>> &cell,
    const unsigned int face_no,
    const unsigned int subface_no);

  /**
   * Alternative reinitialization function that takes, as arguments, iterators
   * to the face and subface instead of their numbers.
   */
  template <bool level_dof_access>
  void
  reinit(
    const TriaIterator<DoFCellAccessor<dim, spacedim, level_dof_access>> &cell,
    const typename Triangulation<dim, spacedim>::face_iterator &          face,
    const typename Triangulation<dim, spacedim>::face_iterator &subface);

  /**
   * Reinitialize the gradients, Jacobi determinants, etc for the given
   * subface on a given cell of type "iterator into a Triangulation object", and
   * the given finite element. Since iterators into a triangulation alone only
   * convey information about the geometry of a cell, but not about degrees of
   * freedom possibly associated with this cell, you will not be able to call
   * some functions of this class if they need information about degrees of
   * freedom. These functions are, above all, the
   * <tt>get_function_value/gradients/hessians/third_derivatives</tt>
   * functions. If you want to call these functions, you have to call the @p
   * reinit variants that take iterators into DoFHandler or other DoF handler
   * type objects.
   */
  void
  reinit(const typename Triangulation<dim, spacedim>::cell_iterator &cell,
         const unsigned int                                          face_no,
         const unsigned int subface_no);

  /**
   * Reinitialize the gradients, Jacobi determinants, etc for the given
   * subface on a given cell of type "iterator into a Triangulation object", and
   * the given finite element. Since iterators into a triangulation alone only
   * convey information about the geometry of a cell, but not about degrees of
   * freedom possibly associated with this cell, you will not be able to call
   * some functions of this class if they need information about degrees of
   * freedom. These functions are, above all, the
   * <tt>get_function_value/gradients/hessians/third_derivatives</tt>
   * functions. If you want to call these functions, you have to call the @p
   * reinit variants that take iterators into DoFHandler or other DoF handler
   * type objects.
   *
   * This does the same thing as the previous function but takes iterators
   * instead of numbers as arguments.
   *
   * @note @p face and @p subface must correspond to a face (and a subface of
   * that face) of @p cell.
   */
  void
  reinit(const typename Triangulation<dim, spacedim>::cell_iterator &cell,
         const typename Triangulation<dim, spacedim>::face_iterator &face,
         const typename Triangulation<dim, spacedim>::face_iterator &subface);

  /**
   * Return a reference to this very object.
   *
   * Though it seems that it is not very useful, this function is there to
   * provide capability to the hp::FEValues class, in which case it provides
   * the FEValues object for the present cell (remember that for hp-finite
   * elements, the actual FE object used may change from cell to cell, so we
   * also need different FEValues objects for different cells; once you
   * reinitialize the hp::FEValues object for a specific cell, it retrieves
   * the FEValues object for the FE on that cell and returns it through a
   * function of the same name as this one; this function here therefore only
   * provides the same interface so that one can templatize on FEValues and
   * hp::FEValues).
   */
  const FESubfaceValues<dim, spacedim> &
  get_present_fe_values() const;

  /**
   * @todo Document this
   *
   * @ingroup Exceptions
   */
  DeclException0(ExcReinitCalledWithBoundaryFace);

  /**
   * @todo Document this
   *
   * @ingroup Exceptions
   */
  DeclException0(ExcFaceHasNoSubfaces);

private:
  /**
   * Do work common to the two constructors.
   */
  void
  initialize(const UpdateFlags update_flags);

  /**
   * The reinit() functions do only that part of the work that requires
   * knowledge of the type of iterator. After setting present_cell(), they
   * pass on to this function, which does the real work, and which is
   * independent of the actual type of the cell iterator.
   */
  void
  do_reinit(const unsigned int face_no, const unsigned int subface_no);
};


#ifndef DOXYGEN


/*------------------------ Inline functions: namespace FEValuesViews --------*/

namespace FEValuesViews
{
  template <int dim, int spacedim>
  inline typename Scalar<dim, spacedim>::value_type
  Scalar<dim, spacedim>::value(const unsigned int shape_function,
                               const unsigned int q_point) const
  {
    AssertIndexRange(shape_function, fe_values->fe->n_dofs_per_cell());
    Assert(
      fe_values->update_flags & update_values,
      ((typename FEValuesBase<dim, spacedim>::ExcAccessToUninitializedField(
        "update_values"))));

    // an adaptation of the FEValuesBase::shape_value_component function
    // except that here we know the component as fixed and we have
    // pre-computed and cached a bunch of information. See the comments there.
    if (shape_function_data[shape_function].is_nonzero_shape_function_component)
      return fe_values->finite_element_output.shape_values(
        shape_function_data[shape_function].row_index, q_point);
    else
      return 0;
  }



  template <int dim, int spacedim>
  inline typename Scalar<dim, spacedim>::gradient_type
  Scalar<dim, spacedim>::gradient(const unsigned int shape_function,
                                  const unsigned int q_point) const
  {
    AssertIndexRange(shape_function, fe_values->fe->n_dofs_per_cell());
    Assert(fe_values->update_flags & update_gradients,
           (typename FEValuesBase<dim, spacedim>::ExcAccessToUninitializedField(
             "update_gradients")));

    // an adaptation of the FEValuesBase::shape_grad_component
    // function except that here we know the component as fixed and we have
    // pre-computed and cached a bunch of information. See the comments there.
    if (shape_function_data[shape_function].is_nonzero_shape_function_component)
      return fe_values->finite_element_output
        .shape_gradients[shape_function_data[shape_function].row_index]
                        [q_point];
    else
      return gradient_type();
  }



  template <int dim, int spacedim>
  inline typename Scalar<dim, spacedim>::hessian_type
  Scalar<dim, spacedim>::hessian(const unsigned int shape_function,
                                 const unsigned int q_point) const
  {
    AssertIndexRange(shape_function, fe_values->fe->n_dofs_per_cell());
    Assert(fe_values->update_flags & update_hessians,
           (typename FEValuesBase<dim, spacedim>::ExcAccessToUninitializedField(
             "update_hessians")));

    // an adaptation of the FEValuesBase::shape_hessian_component
    // function except that here we know the component as fixed and we have
    // pre-computed and cached a bunch of information. See the comments there.
    if (shape_function_data[shape_function].is_nonzero_shape_function_component)
      return fe_values->finite_element_output
        .shape_hessians[shape_function_data[shape_function].row_index][q_point];
    else
      return hessian_type();
  }



  template <int dim, int spacedim>
  inline typename Scalar<dim, spacedim>::third_derivative_type
  Scalar<dim, spacedim>::third_derivative(const unsigned int shape_function,
                                          const unsigned int q_point) const
  {
    AssertIndexRange(shape_function, fe_values->fe->n_dofs_per_cell());
    Assert(fe_values->update_flags & update_3rd_derivatives,
           (typename FEValuesBase<dim, spacedim>::ExcAccessToUninitializedField(
             "update_3rd_derivatives")));

    // an adaptation of the FEValuesBase::shape_3rdderivative_component
    // function except that here we know the component as fixed and we have
    // pre-computed and cached a bunch of information. See the comments there.
    if (shape_function_data[shape_function].is_nonzero_shape_function_component)
      return fe_values->finite_element_output
        .shape_3rd_derivatives[shape_function_data[shape_function].row_index]
                              [q_point];
    else
      return third_derivative_type();
  }



  template <int dim, int spacedim>
  inline typename Vector<dim, spacedim>::value_type
  Vector<dim, spacedim>::value(const unsigned int shape_function,
                               const unsigned int q_point) const
  {
    AssertIndexRange(shape_function, fe_values->fe->n_dofs_per_cell());
    Assert(fe_values->update_flags & update_values,
           (typename FEValuesBase<dim, spacedim>::ExcAccessToUninitializedField(
             "update_values")));

    // same as for the scalar case except that we have one more index
    const int snc =
      shape_function_data[shape_function].single_nonzero_component;
    if (snc == -2)
      return value_type();
    else if (snc != -1)
      {
        value_type return_value;
        return_value[shape_function_data[shape_function]
                       .single_nonzero_component_index] =
          fe_values->finite_element_output.shape_values(snc, q_point);
        return return_value;
      }
    else
      {
        value_type return_value;
        for (unsigned int d = 0; d < dim; ++d)
          if (shape_function_data[shape_function]
                .is_nonzero_shape_function_component[d])
            return_value[d] = fe_values->finite_element_output.shape_values(
              shape_function_data[shape_function].row_index[d], q_point);

        return return_value;
      }
  }



  template <int dim, int spacedim>
  inline typename Vector<dim, spacedim>::gradient_type
  Vector<dim, spacedim>::gradient(const unsigned int shape_function,
                                  const unsigned int q_point) const
  {
    AssertIndexRange(shape_function, fe_values->fe->n_dofs_per_cell());
    Assert(fe_values->update_flags & update_gradients,
           (typename FEValuesBase<dim, spacedim>::ExcAccessToUninitializedField(
             "update_gradients")));

    // same as for the scalar case except that we have one more index
    const int snc =
      shape_function_data[shape_function].single_nonzero_component;
    if (snc == -2)
      return gradient_type();
    else if (snc != -1)
      {
        gradient_type return_value;
        return_value[shape_function_data[shape_function]
                       .single_nonzero_component_index] =
          fe_values->finite_element_output.shape_gradients[snc][q_point];
        return return_value;
      }
    else
      {
        gradient_type return_value;
        for (unsigned int d = 0; d < dim; ++d)
          if (shape_function_data[shape_function]
                .is_nonzero_shape_function_component[d])
            return_value[d] =
              fe_values->finite_element_output.shape_gradients
                [shape_function_data[shape_function].row_index[d]][q_point];

        return return_value;
      }
  }



  template <int dim, int spacedim>
  inline typename Vector<dim, spacedim>::divergence_type
  Vector<dim, spacedim>::divergence(const unsigned int shape_function,
                                    const unsigned int q_point) const
  {
    // this function works like in the case above
    AssertIndexRange(shape_function, fe_values->fe->n_dofs_per_cell());
    Assert(fe_values->update_flags & update_gradients,
           (typename FEValuesBase<dim, spacedim>::ExcAccessToUninitializedField(
             "update_gradients")));

    // same as for the scalar case except that we have one more index
    const int snc =
      shape_function_data[shape_function].single_nonzero_component;
    if (snc == -2)
      return divergence_type();
    else if (snc != -1)
      return fe_values->finite_element_output
        .shape_gradients[snc][q_point][shape_function_data[shape_function]
                                         .single_nonzero_component_index];
    else
      {
        divergence_type return_value = 0;
        for (unsigned int d = 0; d < dim; ++d)
          if (shape_function_data[shape_function]
                .is_nonzero_shape_function_component[d])
            return_value +=
              fe_values->finite_element_output.shape_gradients
                [shape_function_data[shape_function].row_index[d]][q_point][d];

        return return_value;
      }
  }



  template <int dim, int spacedim>
  inline typename Vector<dim, spacedim>::curl_type
  Vector<dim, spacedim>::curl(const unsigned int shape_function,
                              const unsigned int q_point) const
  {
    // this function works like in the case above

    AssertIndexRange(shape_function, fe_values->fe->n_dofs_per_cell());
    Assert(fe_values->update_flags & update_gradients,
           (typename FEValuesBase<dim, spacedim>::ExcAccessToUninitializedField(
             "update_gradients")));
    // same as for the scalar case except that we have one more index
    const int snc =
      shape_function_data[shape_function].single_nonzero_component;

    if (snc == -2)
      return curl_type();

    else
      switch (dim)
        {
          case 1:
            {
              Assert(false,
                     ExcMessage(
                       "Computing the curl in 1d is not a useful operation"));
              return curl_type();
            }

          case 2:
            {
              if (snc != -1)
                {
                  curl_type return_value;

                  // the single nonzero component can only be zero or one in 2d
                  if (shape_function_data[shape_function]
                        .single_nonzero_component_index == 0)
                    return_value[0] =
                      -1.0 * fe_values->finite_element_output
                               .shape_gradients[snc][q_point][1];
                  else
                    return_value[0] = fe_values->finite_element_output
                                        .shape_gradients[snc][q_point][0];

                  return return_value;
                }

              else
                {
                  curl_type return_value;

                  return_value[0] = 0.0;

                  if (shape_function_data[shape_function]
                        .is_nonzero_shape_function_component[0])
                    return_value[0] -=
                      fe_values->finite_element_output
                        .shape_gradients[shape_function_data[shape_function]
                                           .row_index[0]][q_point][1];

                  if (shape_function_data[shape_function]
                        .is_nonzero_shape_function_component[1])
                    return_value[0] +=
                      fe_values->finite_element_output
                        .shape_gradients[shape_function_data[shape_function]
                                           .row_index[1]][q_point][0];

                  return return_value;
                }
            }

          case 3:
            {
              if (snc != -1)
                {
                  curl_type return_value;

                  switch (shape_function_data[shape_function]
                            .single_nonzero_component_index)
                    {
                      case 0:
                        {
                          return_value[0] = 0;
                          return_value[1] = fe_values->finite_element_output
                                              .shape_gradients[snc][q_point][2];
                          return_value[2] =
                            -1.0 * fe_values->finite_element_output
                                     .shape_gradients[snc][q_point][1];
                          return return_value;
                        }

                      case 1:
                        {
                          return_value[0] =
                            -1.0 * fe_values->finite_element_output
                                     .shape_gradients[snc][q_point][2];
                          return_value[1] = 0;
                          return_value[2] = fe_values->finite_element_output
                                              .shape_gradients[snc][q_point][0];
                          return return_value;
                        }

                      default:
                        {
                          return_value[0] = fe_values->finite_element_output
                                              .shape_gradients[snc][q_point][1];
                          return_value[1] =
                            -1.0 * fe_values->finite_element_output
                                     .shape_gradients[snc][q_point][0];
                          return_value[2] = 0;
                          return return_value;
                        }
                    }
                }

              else
                {
                  curl_type return_value;

                  for (unsigned int i = 0; i < dim; ++i)
                    return_value[i] = 0.0;

                  if (shape_function_data[shape_function]
                        .is_nonzero_shape_function_component[0])
                    {
                      return_value[1] +=
                        fe_values->finite_element_output
                          .shape_gradients[shape_function_data[shape_function]
                                             .row_index[0]][q_point][2];
                      return_value[2] -=
                        fe_values->finite_element_output
                          .shape_gradients[shape_function_data[shape_function]
                                             .row_index[0]][q_point][1];
                    }

                  if (shape_function_data[shape_function]
                        .is_nonzero_shape_function_component[1])
                    {
                      return_value[0] -=
                        fe_values->finite_element_output
                          .shape_gradients[shape_function_data[shape_function]
                                             .row_index[1]][q_point][2];
                      return_value[2] +=
                        fe_values->finite_element_output
                          .shape_gradients[shape_function_data[shape_function]
                                             .row_index[1]][q_point][0];
                    }

                  if (shape_function_data[shape_function]
                        .is_nonzero_shape_function_component[2])
                    {
                      return_value[0] +=
                        fe_values->finite_element_output
                          .shape_gradients[shape_function_data[shape_function]
                                             .row_index[2]][q_point][1];
                      return_value[1] -=
                        fe_values->finite_element_output
                          .shape_gradients[shape_function_data[shape_function]
                                             .row_index[2]][q_point][0];
                    }

                  return return_value;
                }
            }
        }
    // should not end up here
    Assert(false, ExcInternalError());
    return curl_type();
  }



  template <int dim, int spacedim>
  inline typename Vector<dim, spacedim>::hessian_type
  Vector<dim, spacedim>::hessian(const unsigned int shape_function,
                                 const unsigned int q_point) const
  {
    // this function works like in the case above
    AssertIndexRange(shape_function, fe_values->fe->n_dofs_per_cell());
    Assert(fe_values->update_flags & update_hessians,
           (typename FEValuesBase<dim, spacedim>::ExcAccessToUninitializedField(
             "update_hessians")));

    // same as for the scalar case except that we have one more index
    const int snc =
      shape_function_data[shape_function].single_nonzero_component;
    if (snc == -2)
      return hessian_type();
    else if (snc != -1)
      {
        hessian_type return_value;
        return_value[shape_function_data[shape_function]
                       .single_nonzero_component_index] =
          fe_values->finite_element_output.shape_hessians[snc][q_point];
        return return_value;
      }
    else
      {
        hessian_type return_value;
        for (unsigned int d = 0; d < dim; ++d)
          if (shape_function_data[shape_function]
                .is_nonzero_shape_function_component[d])
            return_value[d] =
              fe_values->finite_element_output.shape_hessians
                [shape_function_data[shape_function].row_index[d]][q_point];

        return return_value;
      }
  }



  template <int dim, int spacedim>
  inline typename Vector<dim, spacedim>::third_derivative_type
  Vector<dim, spacedim>::third_derivative(const unsigned int shape_function,
                                          const unsigned int q_point) const
  {
    // this function works like in the case above
    AssertIndexRange(shape_function, fe_values->fe->n_dofs_per_cell());
    Assert(fe_values->update_flags & update_3rd_derivatives,
           (typename FEValuesBase<dim, spacedim>::ExcAccessToUninitializedField(
             "update_3rd_derivatives")));

    // same as for the scalar case except that we have one more index
    const int snc =
      shape_function_data[shape_function].single_nonzero_component;
    if (snc == -2)
      return third_derivative_type();
    else if (snc != -1)
      {
        third_derivative_type return_value;
        return_value[shape_function_data[shape_function]
                       .single_nonzero_component_index] =
          fe_values->finite_element_output.shape_3rd_derivatives[snc][q_point];
        return return_value;
      }
    else
      {
        third_derivative_type return_value;
        for (unsigned int d = 0; d < dim; ++d)
          if (shape_function_data[shape_function]
                .is_nonzero_shape_function_component[d])
            return_value[d] =
              fe_values->finite_element_output.shape_3rd_derivatives
                [shape_function_data[shape_function].row_index[d]][q_point];

        return return_value;
      }
  }



  namespace internal
  {
    /**
     * Return the symmetrized version of a tensor whose n'th row equals the
     * second argument, with all other rows equal to zero.
     */
    inline dealii::SymmetricTensor<2, 1>
    symmetrize_single_row(const unsigned int n, const dealii::Tensor<1, 1> &t)
    {
      AssertIndexRange(n, 1);
      (void)n;

      return {{t[0]}};
    }



    inline dealii::SymmetricTensor<2, 2>
    symmetrize_single_row(const unsigned int n, const dealii::Tensor<1, 2> &t)
    {
      switch (n)
        {
          case 0:
            {
              return {{t[0], 0, t[1] / 2}};
            }
          case 1:
            {
              return {{0, t[1], t[0] / 2}};
            }
          default:
            {
              AssertIndexRange(n, 2);
              return {};
            }
        }
    }



    inline dealii::SymmetricTensor<2, 3>
    symmetrize_single_row(const unsigned int n, const dealii::Tensor<1, 3> &t)
    {
      switch (n)
        {
          case 0:
            {
              return {{t[0], 0, 0, t[1] / 2, t[2] / 2, 0}};
            }
          case 1:
            {
              return {{0, t[1], 0, t[0] / 2, 0, t[2] / 2}};
            }
          case 2:
            {
              return {{0, 0, t[2], 0, t[0] / 2, t[1] / 2}};
            }
          default:
            {
              AssertIndexRange(n, 3);
              return {};
            }
        }
    }
  } // namespace internal



  template <int dim, int spacedim>
  inline typename Vector<dim, spacedim>::symmetric_gradient_type
  Vector<dim, spacedim>::symmetric_gradient(const unsigned int shape_function,
                                            const unsigned int q_point) const
  {
    AssertIndexRange(shape_function, fe_values->fe->n_dofs_per_cell());
    Assert(fe_values->update_flags & update_gradients,
           (typename FEValuesBase<dim, spacedim>::ExcAccessToUninitializedField(
             "update_gradients")));

    // same as for the scalar case except that we have one more index
    const int snc =
      shape_function_data[shape_function].single_nonzero_component;
    if (snc == -2)
      return symmetric_gradient_type();
    else if (snc != -1)
      return internal::symmetrize_single_row(
        shape_function_data[shape_function].single_nonzero_component_index,
        fe_values->finite_element_output.shape_gradients[snc][q_point]);
    else
      {
        gradient_type return_value;
        for (unsigned int d = 0; d < dim; ++d)
          if (shape_function_data[shape_function]
                .is_nonzero_shape_function_component[d])
            return_value[d] =
              fe_values->finite_element_output.shape_gradients
                [shape_function_data[shape_function].row_index[d]][q_point];

        return symmetrize(return_value);
      }
  }



  template <int dim, int spacedim>
  inline typename SymmetricTensor<2, dim, spacedim>::value_type
  SymmetricTensor<2, dim, spacedim>::value(const unsigned int shape_function,
                                           const unsigned int q_point) const
  {
    AssertIndexRange(shape_function, fe_values->fe->n_dofs_per_cell());
    Assert(fe_values->update_flags & update_values,
           (typename FEValuesBase<dim, spacedim>::ExcAccessToUninitializedField(
             "update_values")));

    // similar to the vector case where we have more then one index and we need
    // to convert between unrolled and component indexing for tensors
    const int snc =
      shape_function_data[shape_function].single_nonzero_component;

    if (snc == -2)
      {
        // shape function is zero for the selected components
        return value_type();
      }
    else if (snc != -1)
      {
        value_type         return_value;
        const unsigned int comp =
          shape_function_data[shape_function].single_nonzero_component_index;
        return_value[value_type::unrolled_to_component_indices(comp)] =
          fe_values->finite_element_output.shape_values(snc, q_point);
        return return_value;
      }
    else
      {
        value_type return_value;
        for (unsigned int d = 0; d < value_type::n_independent_components; ++d)
          if (shape_function_data[shape_function]
                .is_nonzero_shape_function_component[d])
            return_value[value_type::unrolled_to_component_indices(d)] =
              fe_values->finite_element_output.shape_values(
                shape_function_data[shape_function].row_index[d], q_point);
        return return_value;
      }
  }



  template <int dim, int spacedim>
  inline typename SymmetricTensor<2, dim, spacedim>::divergence_type
  SymmetricTensor<2, dim, spacedim>::divergence(
    const unsigned int shape_function,
    const unsigned int q_point) const
  {
    AssertIndexRange(shape_function, fe_values->fe->n_dofs_per_cell());
    Assert(fe_values->update_flags & update_gradients,
           (typename FEValuesBase<dim, spacedim>::ExcAccessToUninitializedField(
             "update_gradients")));

    const int snc =
      shape_function_data[shape_function].single_nonzero_component;

    if (snc == -2)
      {
        // shape function is zero for the selected components
        return divergence_type();
      }
    else if (snc != -1)
      {
        // we have a single non-zero component when the symmetric tensor is
        // represented in unrolled form. this implies we potentially have
        // two non-zero components when represented in component form!  we
        // will only have one non-zero entry if the non-zero component lies on
        // the diagonal of the tensor.
        //
        // the divergence of a second-order tensor is a first order tensor.
        //
        // assume the second-order tensor is A with components A_{ij}.  then
        // A_{ij} = A_{ji} and there is only one (if diagonal) or two non-zero
        // entries in the tensorial representation.  define the
        // divergence as:
        // b_i \dealcoloneq \dfrac{\partial phi_{ij}}{\partial x_j}.
        // (which is incidentally also
        // b_j \dealcoloneq \dfrac{\partial phi_{ij}}{\partial x_i}).
        // In both cases, a sum is implied.
        //
        // Now, we know the nonzero component in unrolled form: it is indicated
        // by 'snc'. we can figure out which tensor components belong to this:
        const unsigned int comp =
          shape_function_data[shape_function].single_nonzero_component_index;
        const unsigned int ii =
          value_type::unrolled_to_component_indices(comp)[0];
        const unsigned int jj =
          value_type::unrolled_to_component_indices(comp)[1];

        // given the form of the divergence above, if ii=jj there is only a
        // single nonzero component of the full tensor and the gradient
        // equals
        // b_ii \dealcoloneq \dfrac{\partial phi_{ii,ii}}{\partial x_ii}.
        // all other entries of 'b' are zero
        //
        // on the other hand, if ii!=jj, then there are two nonzero entries in
        // the full tensor and
        // b_ii \dealcoloneq \dfrac{\partial phi_{ii,jj}}{\partial x_ii}.
        // b_jj \dealcoloneq \dfrac{\partial phi_{ii,jj}}{\partial x_jj}.
        // again, all other entries of 'b' are zero
        const dealii::Tensor<1, spacedim> &phi_grad =
          fe_values->finite_element_output.shape_gradients[snc][q_point];

        divergence_type return_value;
        return_value[ii] = phi_grad[jj];

        if (ii != jj)
          return_value[jj] = phi_grad[ii];

        return return_value;
      }
    else
      {
        Assert(false, ExcNotImplemented());
        divergence_type return_value;
        return return_value;
      }
  }



  template <int dim, int spacedim>
  inline typename Tensor<2, dim, spacedim>::value_type
  Tensor<2, dim, spacedim>::value(const unsigned int shape_function,
                                  const unsigned int q_point) const
  {
    AssertIndexRange(shape_function, fe_values->fe->n_dofs_per_cell());
    Assert(fe_values->update_flags & update_values,
           (typename FEValuesBase<dim, spacedim>::ExcAccessToUninitializedField(
             "update_values")));

    // similar to the vector case where we have more then one index and we need
    // to convert between unrolled and component indexing for tensors
    const int snc =
      shape_function_data[shape_function].single_nonzero_component;

    if (snc == -2)
      {
        // shape function is zero for the selected components
        return value_type();
      }
    else if (snc != -1)
      {
        value_type         return_value;
        const unsigned int comp =
          shape_function_data[shape_function].single_nonzero_component_index;
        const TableIndices<2> indices =
          dealii::Tensor<2, spacedim>::unrolled_to_component_indices(comp);
        return_value[indices] =
          fe_values->finite_element_output.shape_values(snc, q_point);
        return return_value;
      }
    else
      {
        value_type return_value;
        for (unsigned int d = 0; d < dim * dim; ++d)
          if (shape_function_data[shape_function]
                .is_nonzero_shape_function_component[d])
            {
              const TableIndices<2> indices =
                dealii::Tensor<2, spacedim>::unrolled_to_component_indices(d);
              return_value[indices] =
                fe_values->finite_element_output.shape_values(
                  shape_function_data[shape_function].row_index[d], q_point);
            }
        return return_value;
      }
  }



  template <int dim, int spacedim>
  inline typename Tensor<2, dim, spacedim>::divergence_type
  Tensor<2, dim, spacedim>::divergence(const unsigned int shape_function,
                                       const unsigned int q_point) const
  {
    AssertIndexRange(shape_function, fe_values->fe->n_dofs_per_cell());
    Assert(fe_values->update_flags & update_gradients,
           (typename FEValuesBase<dim, spacedim>::ExcAccessToUninitializedField(
             "update_gradients")));

    const int snc =
      shape_function_data[shape_function].single_nonzero_component;

    if (snc == -2)
      {
        // shape function is zero for the selected components
        return divergence_type();
      }
    else if (snc != -1)
      {
        // we have a single non-zero component when the tensor is
        // represented in unrolled form.
        //
        // the divergence of a second-order tensor is a first order tensor.
        //
        // assume the second-order tensor is A with components A_{ij},
        // then divergence is d_i := \frac{\partial A_{ij}}{\partial x_j}
        //
        // Now, we know the nonzero component in unrolled form: it is indicated
        // by 'snc'. we can figure out which tensor components belong to this:
        const unsigned int comp =
          shape_function_data[shape_function].single_nonzero_component_index;
        const TableIndices<2> indices =
          dealii::Tensor<2, spacedim>::unrolled_to_component_indices(comp);
        const unsigned int ii = indices[0];
        const unsigned int jj = indices[1];

        const dealii::Tensor<1, spacedim> &phi_grad =
          fe_values->finite_element_output.shape_gradients[snc][q_point];

        divergence_type return_value;
        // note that we contract \nabla from the right
        return_value[ii] = phi_grad[jj];

        return return_value;
      }
    else
      {
        Assert(false, ExcNotImplemented());
        divergence_type return_value;
        return return_value;
      }
  }



  template <int dim, int spacedim>
  inline typename Tensor<2, dim, spacedim>::gradient_type
  Tensor<2, dim, spacedim>::gradient(const unsigned int shape_function,
                                     const unsigned int q_point) const
  {
    AssertIndexRange(shape_function, fe_values->fe->n_dofs_per_cell());
    Assert(fe_values->update_flags & update_gradients,
           (typename FEValuesBase<dim, spacedim>::ExcAccessToUninitializedField(
             "update_gradients")));

    const int snc =
      shape_function_data[shape_function].single_nonzero_component;

    if (snc == -2)
      {
        // shape function is zero for the selected components
        return gradient_type();
      }
    else if (snc != -1)
      {
        // we have a single non-zero component when the tensor is
        // represented in unrolled form.
        //
        // the gradient of a second-order tensor is a third order tensor.
        //
        // assume the second-order tensor is A with components A_{ij},
        // then gradient is B_{ijk} := \frac{\partial A_{ij}}{\partial x_k}
        //
        // Now, we know the nonzero component in unrolled form: it is indicated
        // by 'snc'. we can figure out which tensor components belong to this:
        const unsigned int comp =
          shape_function_data[shape_function].single_nonzero_component_index;
        const TableIndices<2> indices =
          dealii::Tensor<2, spacedim>::unrolled_to_component_indices(comp);
        const unsigned int ii = indices[0];
        const unsigned int jj = indices[1];

        const dealii::Tensor<1, spacedim> &phi_grad =
          fe_values->finite_element_output.shape_gradients[snc][q_point];

        gradient_type return_value;
        return_value[ii][jj] = phi_grad;

        return return_value;
      }
    else
      {
        Assert(false, ExcNotImplemented());
        gradient_type return_value;
        return return_value;
      }
  }

} // namespace FEValuesViews



/*---------------------- Inline functions: FEValuesBase ---------------------*/



template <int dim, int spacedim>
template <bool lda>
inline FEValuesBase<dim, spacedim>::CellIteratorContainer::
  CellIteratorContainer(
    const TriaIterator<DoFCellAccessor<dim, spacedim, lda>> &cell)
  : initialized(true)
  , cell(cell)
  , dof_handler(&cell->get_dof_handler())
  , level_dof_access(lda)
{}



template <int dim, int spacedim>
inline const FEValuesViews::Scalar<dim, spacedim> &
FEValuesBase<dim, spacedim>::operator[](
  const FEValuesExtractors::Scalar &scalar) const
{
  AssertIndexRange(scalar.component, fe_values_views_cache.scalars.size());

  return fe_values_views_cache.scalars[scalar.component];
}



template <int dim, int spacedim>
inline const FEValuesViews::Vector<dim, spacedim> &
FEValuesBase<dim, spacedim>::operator[](
  const FEValuesExtractors::Vector &vector) const
{
  AssertIndexRange(vector.first_vector_component,
                   fe_values_views_cache.vectors.size());

  return fe_values_views_cache.vectors[vector.first_vector_component];
}



template <int dim, int spacedim>
inline const FEValuesViews::SymmetricTensor<2, dim, spacedim> &
FEValuesBase<dim, spacedim>::operator[](
  const FEValuesExtractors::SymmetricTensor<2> &tensor) const
{
  Assert(
    tensor.first_tensor_component <
      fe_values_views_cache.symmetric_second_order_tensors.size(),
    ExcIndexRange(tensor.first_tensor_component,
                  0,
                  fe_values_views_cache.symmetric_second_order_tensors.size()));

  return fe_values_views_cache
    .symmetric_second_order_tensors[tensor.first_tensor_component];
}



template <int dim, int spacedim>
inline const FEValuesViews::Tensor<2, dim, spacedim> &
FEValuesBase<dim, spacedim>::operator[](
  const FEValuesExtractors::Tensor<2> &tensor) const
{
  AssertIndexRange(tensor.first_tensor_component,
                   fe_values_views_cache.second_order_tensors.size());

  return fe_values_views_cache
    .second_order_tensors[tensor.first_tensor_component];
}



template <int dim, int spacedim>
inline const double &
FEValuesBase<dim, spacedim>::shape_value(const unsigned int i,
                                         const unsigned int j) const
{
  AssertIndexRange(i, fe->n_dofs_per_cell());
  Assert(this->update_flags & update_values,
         ExcAccessToUninitializedField("update_values"));
  Assert(fe->is_primitive(i), ExcShapeFunctionNotPrimitive(i));
  Assert(present_cell.is_initialized(), ExcNotReinited());
  // if the entire FE is primitive,
  // then we can take a short-cut:
  if (fe->is_primitive())
    return this->finite_element_output.shape_values(i, j);
  else
    {
      // otherwise, use the mapping
      // between shape function
      // numbers and rows. note that
      // by the assertions above, we
      // know that this particular
      // shape function is primitive,
      // so we can call
      // system_to_component_index
      const unsigned int row =
        this->finite_element_output
          .shape_function_to_row_table[i * fe->n_components() +
                                       fe->system_to_component_index(i).first];
      return this->finite_element_output.shape_values(row, j);
    }
}



template <int dim, int spacedim>
inline double
FEValuesBase<dim, spacedim>::shape_value_component(
  const unsigned int i,
  const unsigned int j,
  const unsigned int component) const
{
  AssertIndexRange(i, fe->n_dofs_per_cell());
  Assert(this->update_flags & update_values,
         ExcAccessToUninitializedField("update_values"));
  AssertIndexRange(component, fe->n_components());
  Assert(present_cell.is_initialized(), ExcNotReinited());

  // check whether the shape function
  // is non-zero at all within
  // this component:
  if (fe->get_nonzero_components(i)[component] == false)
    return 0;

  // look up the right row in the
  // table and take the data from
  // there
  const unsigned int row =
    this->finite_element_output
      .shape_function_to_row_table[i * fe->n_components() + component];
  return this->finite_element_output.shape_values(row, j);
}



template <int dim, int spacedim>
inline const Tensor<1, spacedim> &
FEValuesBase<dim, spacedim>::shape_grad(const unsigned int i,
                                        const unsigned int j) const
{
  AssertIndexRange(i, fe->n_dofs_per_cell());
  Assert(this->update_flags & update_gradients,
         ExcAccessToUninitializedField("update_gradients"));
  Assert(fe->is_primitive(i), ExcShapeFunctionNotPrimitive(i));
  Assert(present_cell.is_initialized(), ExcNotReinited());
  // if the entire FE is primitive,
  // then we can take a short-cut:
  if (fe->is_primitive())
    return this->finite_element_output.shape_gradients[i][j];
  else
    {
      // otherwise, use the mapping
      // between shape function
      // numbers and rows. note that
      // by the assertions above, we
      // know that this particular
      // shape function is primitive,
      // so we can call
      // system_to_component_index
      const unsigned int row =
        this->finite_element_output
          .shape_function_to_row_table[i * fe->n_components() +
                                       fe->system_to_component_index(i).first];
      return this->finite_element_output.shape_gradients[row][j];
    }
}



template <int dim, int spacedim>
inline Tensor<1, spacedim>
FEValuesBase<dim, spacedim>::shape_grad_component(
  const unsigned int i,
  const unsigned int j,
  const unsigned int component) const
{
  AssertIndexRange(i, fe->n_dofs_per_cell());
  Assert(this->update_flags & update_gradients,
         ExcAccessToUninitializedField("update_gradients"));
  AssertIndexRange(component, fe->n_components());
  Assert(present_cell.is_initialized(), ExcNotReinited());
  // check whether the shape function
  // is non-zero at all within
  // this component:
  if (fe->get_nonzero_components(i)[component] == false)
    return Tensor<1, spacedim>();

  // look up the right row in the
  // table and take the data from
  // there
  const unsigned int row =
    this->finite_element_output
      .shape_function_to_row_table[i * fe->n_components() + component];
  return this->finite_element_output.shape_gradients[row][j];
}



template <int dim, int spacedim>
inline const Tensor<2, spacedim> &
FEValuesBase<dim, spacedim>::shape_hessian(const unsigned int i,
                                           const unsigned int j) const
{
  AssertIndexRange(i, fe->n_dofs_per_cell());
  Assert(this->update_flags & update_hessians,
         ExcAccessToUninitializedField("update_hessians"));
  Assert(fe->is_primitive(i), ExcShapeFunctionNotPrimitive(i));
  Assert(present_cell.is_initialized(), ExcNotReinited());
  // if the entire FE is primitive,
  // then we can take a short-cut:
  if (fe->is_primitive())
    return this->finite_element_output.shape_hessians[i][j];
  else
    {
      // otherwise, use the mapping
      // between shape function
      // numbers and rows. note that
      // by the assertions above, we
      // know that this particular
      // shape function is primitive,
      // so we can call
      // system_to_component_index
      const unsigned int row =
        this->finite_element_output
          .shape_function_to_row_table[i * fe->n_components() +
                                       fe->system_to_component_index(i).first];
      return this->finite_element_output.shape_hessians[row][j];
    }
}



template <int dim, int spacedim>
inline Tensor<2, spacedim>
FEValuesBase<dim, spacedim>::shape_hessian_component(
  const unsigned int i,
  const unsigned int j,
  const unsigned int component) const
{
  AssertIndexRange(i, fe->n_dofs_per_cell());
  Assert(this->update_flags & update_hessians,
         ExcAccessToUninitializedField("update_hessians"));
  AssertIndexRange(component, fe->n_components());
  Assert(present_cell.is_initialized(), ExcNotReinited());
  // check whether the shape function
  // is non-zero at all within
  // this component:
  if (fe->get_nonzero_components(i)[component] == false)
    return Tensor<2, spacedim>();

  // look up the right row in the
  // table and take the data from
  // there
  const unsigned int row =
    this->finite_element_output
      .shape_function_to_row_table[i * fe->n_components() + component];
  return this->finite_element_output.shape_hessians[row][j];
}



template <int dim, int spacedim>
inline const Tensor<3, spacedim> &
FEValuesBase<dim, spacedim>::shape_3rd_derivative(const unsigned int i,
                                                  const unsigned int j) const
{
  AssertIndexRange(i, fe->n_dofs_per_cell());
  Assert(this->update_flags & update_3rd_derivatives,
         ExcAccessToUninitializedField("update_3rd_derivatives"));
  Assert(fe->is_primitive(i), ExcShapeFunctionNotPrimitive(i));
  Assert(present_cell.is_initialized(), ExcNotReinited());
  // if the entire FE is primitive,
  // then we can take a short-cut:
  if (fe->is_primitive())
    return this->finite_element_output.shape_3rd_derivatives[i][j];
  else
    {
      // otherwise, use the mapping
      // between shape function
      // numbers and rows. note that
      // by the assertions above, we
      // know that this particular
      // shape function is primitive,
      // so we can call
      // system_to_component_index
      const unsigned int row =
        this->finite_element_output
          .shape_function_to_row_table[i * fe->n_components() +
                                       fe->system_to_component_index(i).first];
      return this->finite_element_output.shape_3rd_derivatives[row][j];
    }
}



template <int dim, int spacedim>
inline Tensor<3, spacedim>
FEValuesBase<dim, spacedim>::shape_3rd_derivative_component(
  const unsigned int i,
  const unsigned int j,
  const unsigned int component) const
{
  AssertIndexRange(i, fe->n_dofs_per_cell());
  Assert(this->update_flags & update_3rd_derivatives,
         ExcAccessToUninitializedField("update_3rd_derivatives"));
  AssertIndexRange(component, fe->n_components());
  Assert(present_cell.is_initialized(), ExcNotReinited());
  // check whether the shape function
  // is non-zero at all within
  // this component:
  if (fe->get_nonzero_components(i)[component] == false)
    return Tensor<3, spacedim>();

  // look up the right row in the
  // table and take the data from
  // there
  const unsigned int row =
    this->finite_element_output
      .shape_function_to_row_table[i * fe->n_components() + component];
  return this->finite_element_output.shape_3rd_derivatives[row][j];
}



template <int dim, int spacedim>
inline const FiniteElement<dim, spacedim> &
FEValuesBase<dim, spacedim>::get_fe() const
{
  return *fe;
}



template <int dim, int spacedim>
inline const Mapping<dim, spacedim> &
FEValuesBase<dim, spacedim>::get_mapping() const
{
  return *mapping;
}



template <int dim, int spacedim>
inline UpdateFlags
FEValuesBase<dim, spacedim>::get_update_flags() const
{
  return this->update_flags;
}



template <int dim, int spacedim>
inline const std::vector<Point<spacedim>> &
FEValuesBase<dim, spacedim>::get_quadrature_points() const
{
  Assert(this->update_flags & update_quadrature_points,
         ExcAccessToUninitializedField("update_quadrature_points"));
  Assert(present_cell.is_initialized(), ExcNotReinited());
  return this->mapping_output.quadrature_points;
}



template <int dim, int spacedim>
inline const std::vector<double> &
FEValuesBase<dim, spacedim>::get_JxW_values() const
{
  Assert(this->update_flags & update_JxW_values,
         ExcAccessToUninitializedField("update_JxW_values"));
  Assert(present_cell.is_initialized(), ExcNotReinited());
  return this->mapping_output.JxW_values;
}



template <int dim, int spacedim>
inline const std::vector<DerivativeForm<1, dim, spacedim>> &
FEValuesBase<dim, spacedim>::get_jacobians() const
{
  Assert(this->update_flags & update_jacobians,
         ExcAccessToUninitializedField("update_jacobians"));
  Assert(present_cell.is_initialized(), ExcNotReinited());
  return this->mapping_output.jacobians;
}



template <int dim, int spacedim>
inline const std::vector<DerivativeForm<2, dim, spacedim>> &
FEValuesBase<dim, spacedim>::get_jacobian_grads() const
{
  Assert(this->update_flags & update_jacobian_grads,
         ExcAccessToUninitializedField("update_jacobians_grads"));
  Assert(present_cell.is_initialized(), ExcNotReinited());
  return this->mapping_output.jacobian_grads;
}



template <int dim, int spacedim>
inline const Tensor<3, spacedim> &
FEValuesBase<dim, spacedim>::jacobian_pushed_forward_grad(
  const unsigned int i) const
{
  Assert(this->update_flags & update_jacobian_pushed_forward_grads,
         ExcAccessToUninitializedField("update_jacobian_pushed_forward_grads"));
  Assert(present_cell.is_initialized(), ExcNotReinited());
  return this->mapping_output.jacobian_pushed_forward_grads[i];
}



template <int dim, int spacedim>
inline const std::vector<Tensor<3, spacedim>> &
FEValuesBase<dim, spacedim>::get_jacobian_pushed_forward_grads() const
{
  Assert(this->update_flags & update_jacobian_pushed_forward_grads,
         ExcAccessToUninitializedField("update_jacobian_pushed_forward_grads"));
  Assert(present_cell.is_initialized(), ExcNotReinited());
  return this->mapping_output.jacobian_pushed_forward_grads;
}



template <int dim, int spacedim>
inline const DerivativeForm<3, dim, spacedim> &
FEValuesBase<dim, spacedim>::jacobian_2nd_derivative(const unsigned int i) const
{
  Assert(this->update_flags & update_jacobian_2nd_derivatives,
         ExcAccessToUninitializedField("update_jacobian_2nd_derivatives"));
  Assert(present_cell.is_initialized(), ExcNotReinited());
  return this->mapping_output.jacobian_2nd_derivatives[i];
}



template <int dim, int spacedim>
inline const std::vector<DerivativeForm<3, dim, spacedim>> &
FEValuesBase<dim, spacedim>::get_jacobian_2nd_derivatives() const
{
  Assert(this->update_flags & update_jacobian_2nd_derivatives,
         ExcAccessToUninitializedField("update_jacobian_2nd_derivatives"));
  Assert(present_cell.is_initialized(), ExcNotReinited());
  return this->mapping_output.jacobian_2nd_derivatives;
}



template <int dim, int spacedim>
inline const Tensor<4, spacedim> &
FEValuesBase<dim, spacedim>::jacobian_pushed_forward_2nd_derivative(
  const unsigned int i) const
{
  Assert(this->update_flags & update_jacobian_pushed_forward_2nd_derivatives,
         ExcAccessToUninitializedField(
           "update_jacobian_pushed_forward_2nd_derivatives"));
  Assert(present_cell.is_initialized(), ExcNotReinited());
  return this->mapping_output.jacobian_pushed_forward_2nd_derivatives[i];
}



template <int dim, int spacedim>
inline const std::vector<Tensor<4, spacedim>> &
FEValuesBase<dim, spacedim>::get_jacobian_pushed_forward_2nd_derivatives() const
{
  Assert(this->update_flags & update_jacobian_pushed_forward_2nd_derivatives,
         ExcAccessToUninitializedField(
           "update_jacobian_pushed_forward_2nd_derivatives"));
  Assert(present_cell.is_initialized(), ExcNotReinited());
  return this->mapping_output.jacobian_pushed_forward_2nd_derivatives;
}



template <int dim, int spacedim>
inline const DerivativeForm<4, dim, spacedim> &
FEValuesBase<dim, spacedim>::jacobian_3rd_derivative(const unsigned int i) const
{
  Assert(this->update_flags & update_jacobian_3rd_derivatives,
         ExcAccessToUninitializedField("update_jacobian_3rd_derivatives"));
  Assert(present_cell.is_initialized(), ExcNotReinited());
  return this->mapping_output.jacobian_3rd_derivatives[i];
}



template <int dim, int spacedim>
inline const std::vector<DerivativeForm<4, dim, spacedim>> &
FEValuesBase<dim, spacedim>::get_jacobian_3rd_derivatives() const
{
  Assert(this->update_flags & update_jacobian_3rd_derivatives,
         ExcAccessToUninitializedField("update_jacobian_3rd_derivatives"));
  Assert(present_cell.is_initialized(), ExcNotReinited());
  return this->mapping_output.jacobian_3rd_derivatives;
}



template <int dim, int spacedim>
inline const Tensor<5, spacedim> &
FEValuesBase<dim, spacedim>::jacobian_pushed_forward_3rd_derivative(
  const unsigned int i) const
{
  Assert(this->update_flags & update_jacobian_pushed_forward_3rd_derivatives,
         ExcAccessToUninitializedField(
           "update_jacobian_pushed_forward_3rd_derivatives"));
  Assert(present_cell.is_initialized(), ExcNotReinited());
  return this->mapping_output.jacobian_pushed_forward_3rd_derivatives[i];
}



template <int dim, int spacedim>
inline const std::vector<Tensor<5, spacedim>> &
FEValuesBase<dim, spacedim>::get_jacobian_pushed_forward_3rd_derivatives() const
{
  Assert(this->update_flags & update_jacobian_pushed_forward_3rd_derivatives,
         ExcAccessToUninitializedField(
           "update_jacobian_pushed_forward_3rd_derivatives"));
  Assert(present_cell.is_initialized(), ExcNotReinited());
  return this->mapping_output.jacobian_pushed_forward_3rd_derivatives;
}



template <int dim, int spacedim>
inline const std::vector<DerivativeForm<1, spacedim, dim>> &
FEValuesBase<dim, spacedim>::get_inverse_jacobians() const
{
  Assert(this->update_flags & update_inverse_jacobians,
         ExcAccessToUninitializedField("update_inverse_jacobians"));
  Assert(present_cell.is_initialized(), ExcNotReinited());
  return this->mapping_output.inverse_jacobians;
}



template <int dim, int spacedim>
inline std_cxx20::ranges::iota_view<unsigned int, unsigned int>
FEValuesBase<dim, spacedim>::dof_indices() const
{
  return {0U, dofs_per_cell};
}



template <int dim, int spacedim>
inline std_cxx20::ranges::iota_view<unsigned int, unsigned int>
FEValuesBase<dim, spacedim>::dof_indices_starting_at(
  const unsigned int start_dof_index) const
{
  Assert(start_dof_index <= dofs_per_cell,
         ExcIndexRange(start_dof_index, 0, dofs_per_cell + 1));
  return {start_dof_index, dofs_per_cell};
}



template <int dim, int spacedim>
inline std_cxx20::ranges::iota_view<unsigned int, unsigned int>
FEValuesBase<dim, spacedim>::dof_indices_ending_at(
  const unsigned int end_dof_index) const
{
  Assert(end_dof_index < dofs_per_cell,
         ExcIndexRange(end_dof_index, 0, dofs_per_cell));
  return {0U, end_dof_index + 1};
}



template <int dim, int spacedim>
inline std_cxx20::ranges::iota_view<unsigned int, unsigned int>
FEValuesBase<dim, spacedim>::quadrature_point_indices() const
{
  return {0U, n_quadrature_points};
}



template <int dim, int spacedim>
inline const Point<spacedim> &
FEValuesBase<dim, spacedim>::quadrature_point(const unsigned int i) const
{
  Assert(this->update_flags & update_quadrature_points,
         ExcAccessToUninitializedField("update_quadrature_points"));
  AssertIndexRange(i, this->mapping_output.quadrature_points.size());
  Assert(present_cell.is_initialized(), ExcNotReinited());

  return this->mapping_output.quadrature_points[i];
}



template <int dim, int spacedim>
inline double
FEValuesBase<dim, spacedim>::JxW(const unsigned int i) const
{
  Assert(this->update_flags & update_JxW_values,
         ExcAccessToUninitializedField("update_JxW_values"));
  AssertIndexRange(i, this->mapping_output.JxW_values.size());
  Assert(present_cell.is_initialized(), ExcNotReinited());

  return this->mapping_output.JxW_values[i];
}



template <int dim, int spacedim>
inline const DerivativeForm<1, dim, spacedim> &
FEValuesBase<dim, spacedim>::jacobian(const unsigned int i) const
{
  Assert(this->update_flags & update_jacobians,
         ExcAccessToUninitializedField("update_jacobians"));
  AssertIndexRange(i, this->mapping_output.jacobians.size());
  Assert(present_cell.is_initialized(), ExcNotReinited());

  return this->mapping_output.jacobians[i];
}



template <int dim, int spacedim>
inline const DerivativeForm<2, dim, spacedim> &
FEValuesBase<dim, spacedim>::jacobian_grad(const unsigned int i) const
{
  Assert(this->update_flags & update_jacobian_grads,
         ExcAccessToUninitializedField("update_jacobians_grads"));
  AssertIndexRange(i, this->mapping_output.jacobian_grads.size());
  Assert(present_cell.is_initialized(), ExcNotReinited());

  return this->mapping_output.jacobian_grads[i];
}



template <int dim, int spacedim>
inline const DerivativeForm<1, spacedim, dim> &
FEValuesBase<dim, spacedim>::inverse_jacobian(const unsigned int i) const
{
  Assert(this->update_flags & update_inverse_jacobians,
         ExcAccessToUninitializedField("update_inverse_jacobians"));
  AssertIndexRange(i, this->mapping_output.inverse_jacobians.size());
  Assert(present_cell.is_initialized(), ExcNotReinited());

  return this->mapping_output.inverse_jacobians[i];
}



template <int dim, int spacedim>
inline const Tensor<1, spacedim> &
FEValuesBase<dim, spacedim>::normal_vector(const unsigned int i) const
{
  Assert(this->update_flags & update_normal_vectors,
         (typename FEValuesBase<dim, spacedim>::ExcAccessToUninitializedField(
           "update_normal_vectors")));
  AssertIndexRange(i, this->mapping_output.normal_vectors.size());
  Assert(present_cell.is_initialized(), ExcNotReinited());

  return this->mapping_output.normal_vectors[i];
}



/*--------------------- Inline functions: FEValues --------------------------*/


template <int dim, int spacedim>
inline const Quadrature<dim> &
FEValues<dim, spacedim>::get_quadrature() const
{
  return quadrature;
}



template <int dim, int spacedim>
inline const FEValues<dim, spacedim> &
FEValues<dim, spacedim>::get_present_fe_values() const
{
  return *this;
}


/*---------------------- Inline functions: FEFaceValuesBase -----------------*/


template <int dim, int spacedim>
inline unsigned int
FEFaceValuesBase<dim, spacedim>::get_face_number() const
{
  return present_face_no;
}


template <int dim, int spacedim>
inline unsigned int
FEFaceValuesBase<dim, spacedim>::get_face_index() const
{
  return present_face_index;
}


/*----------------------- Inline functions: FE*FaceValues -------------------*/

template <int dim, int spacedim>
inline const Quadrature<dim - 1> &
FEFaceValuesBase<dim, spacedim>::get_quadrature() const
{
  return quadrature[quadrature.size() == 1 ? 0 : present_face_no];
}



template <int dim, int spacedim>
inline const FEFaceValues<dim, spacedim> &
FEFaceValues<dim, spacedim>::get_present_fe_values() const
{
  return *this;
}



template <int dim, int spacedim>
inline const FESubfaceValues<dim, spacedim> &
FESubfaceValues<dim, spacedim>::get_present_fe_values() const
{
  return *this;
}



template <int dim, int spacedim>
inline const Tensor<1, spacedim> &
FEFaceValuesBase<dim, spacedim>::boundary_form(const unsigned int i) const
{
  AssertIndexRange(i, this->mapping_output.boundary_forms.size());
  Assert(this->update_flags & update_boundary_forms,
         (typename FEValuesBase<dim, spacedim>::ExcAccessToUninitializedField(
           "update_boundary_forms")));

  return this->mapping_output.boundary_forms[i];
}

#endif // DOXYGEN

DEAL_II_NAMESPACE_CLOSE

#endif
