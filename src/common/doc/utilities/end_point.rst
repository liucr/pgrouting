.. 
   ****************************************************************************
    pgRouting Manual
    Copyright(c) pgRouting Contributors

    This documentation is licensed under a Creative Commons Attribution-Share
    Alike 3.0 License: http://creativecommons.org/licenses/by-sa/3.0/
   ****************************************************************************

.. _pgr_end_point:

pgr_endPoint - Deprecated Function
===============================================================================

.. warning:: This function is deprecated!!!

    * Is no longer supported.
    * May be removed from future versions.
    * There is no replacement.

Name
-------------------------------------------------------------------------------

``pgr_endPoint`` — Returns an end point of a (multi)linestring geometry.

.. note:: This function is intended for the developer's aid. 


Synopsis
-------------------------------------------------------------------------------

Returns the geometry of the end point of the first LINESTRING of ``geom``. 

.. code-block:: sql

	text pgr_startPoint(geometry geom);


Description
-------------------------------------------------------------------------------

.. rubric:: Parameters

:geom: ``geometry`` Geometry of a MULTILINESTRING or LINESTRING.

Returns the geometry of the end point of the first LINESTRING of ``geom``. 



.. rubric:: History

* New in version 2.0.0



See Also
-------------------------------------------------------------------------------

* :ref:`developer` for the tree layout of the project.
* :ref:`pgr_start_point` to get the start point of a (multi)linestring.

