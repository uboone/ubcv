/**
 * \file pixeldata.h
 *
 * \ingroup ubdldata
 *
 * \brief Class def header for a class pixeldata
 *
 * @author T. Wongjirad 2019/12
 */

/** \addtogroup ubdldata

    @{*/
#ifndef PIXELDATA_H
#define PIXELDATA_H

#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>


namespace ubdldata {

  /**
     \class pixeldata
     Stores maps between ordered pairs (x,y) and a vector of floats
     The intention is to store information per pixel of an image.
     Can provide a bounding box for an image if needed, and the subdata returned.
     
     One application we keep in mind, is the need to have to generate this truth information
       for a crop, or sub-region, of an image. This means, the bounding box and mask have to 
       be modified when the crop only contains a portion of the object.

     We allow the fact that there is a 2D coordinate system overlaid on the 2D array

     Points provided are assumed to be (x,y) <--> (col,row)
     However, when returning dense arrays, the values are in (row,col) order to match
       larcv::Image2D data arrays.
    
  */

  class pixeldata {
    
  public:
    /// default constructor
    pixeldata(); 

    /// constructor with all inputs prepared
    pixeldata( std::vector< std::vector<float> >& data_v,
	       float meta_xmin, float meta_ymin, float meta_xmax, float meta_ymax,
	       int ncols, int nrows, int id=-1, int dim_per_point=2, int label=0, 
	       bool data_is_array_indices=false );
    virtual ~pixeldata() {}

    /// return number of (x,y) points which have data
    int   len() const { return _data_v.size()/_dim_per_point; };

    /// amount of data per point
    int   dim_per_point() const { return _dim_per_point; };

    /// columns (if data represented by dense array)
    int   cols() const { return _ncols; };

    /// rows (if data represented by dense array)
    int   rows() const { return _nrows; };

    /// is currently empty
    bool  isempty() const { return _dim_per_point==0; };
    
    /// get the x position given a column index
    float pos_x( int col, bool checkbounds=false ) const;

    /// get the y-position given a row index
    float pos_y( int row, bool checkbounds=false ) const;

    /// get column given x-coordinate
    int   col( float x, bool checkbounds=false ) const;

    /// get row given y-coordinate
    int   row( float y, bool checkbounds=false ) const;

    /// get id number (example, use for plane ID)
    int  id() const { return _id; };

    /// check if region represented contains (x,y)
    bool  contains( float x, float y ) const;

    /// check if (col,row) is within region (if data represented by dense array)
    bool  contains_index( int col, int row ) const;
    
    /// point, given index
    std::vector<float> point( int index )    const;

    /// pixel pos (if data presented by dense array) given index
    std::vector<int>   pixelpos( int index ) const;

    // mask-generations: dense vector
    std::vector<float> as_vector_mask( float meta_xmin, float meta_ymin, float meta_xmax, float meta_ymax, int ncols, int nrows ) const; // mask provided image info
    std::vector<float> as_vector_mask() const; // dense mask embedded in image coords provided at constructor

    // returning the bounding boxes    
    std::vector<float> as_vector_bbox() const;
    std::vector<float> as_vector_bbox( float meta_xmin, float meta_ymin, float meta_xmax, float meta_ymax ) const; // bounding box constrained within crop
    
    std::vector<int>   as_vector_array_bounds() const;    
    std::vector<int>   as_vector_array_bounds( float meta_xmin, float meta_ymin, float meta_xmax, float meta_ymax ) const; // mask provided image info


    
  protected:

    int _dim_per_point;
    int _label;
    std::vector< float > _data_v; // unrolled
    std::vector< float>  _bbox_v; // (xmin, ymin, xmax, ymax)

    // meta: translation between overlaid coordinate system and image array
    // origin (xmin,ymin) is (0,0) in the data array
    float _meta_xmin;
    float _meta_ymin;
    float _meta_xmax;
    float _meta_ymax;
    int   _ncols;
    int   _nrows;
    int   _id;
    float _pixel_width;
    float _pixel_height;
    

  };
  //End of Class  
  
}//end of namespace
#endif
/** @} */ // end of doxygen group
