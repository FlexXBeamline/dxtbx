/*
 * beam.h
 *
 *  Copyright (C) 2013 Diamond Light Source
 *
 *  Author: James Parkhurst
 *
 *  This code is distributed under the BSD license, a copy of which is
 *  included in the root directory of this package.
 */
#ifndef DXTBX_MODEL_BEAM_H
#define DXTBX_MODEL_BEAM_H

#include <iostream>
#include <cmath>
#include <string>
#include <scitbx/vec3.h>
#include <scitbx/array_family/shared.h>
#include <scitbx/array_family/simple_io.h>
#include <scitbx/array_family/simple_tiny_io.h>
#include <dxtbx/error.h>
#include "model_helpers.h"

namespace dxtbx { namespace model {

  using scitbx::vec3;

  // probe type enumeration
  enum Probe { xray = 1, electron = 2, neutron = 3 };

  /** Base class for beam objects */
  class BeamBase {
  public:
    virtual ~BeamBase() {}

    virtual vec3<double> get_sample_to_source_direction() const = 0;
    virtual double get_sample_to_source_distance() const = 0;
    virtual double get_wavelength() const = 0;
    virtual double get_divergence() const = 0;
    // Get the standard deviation of the beam divergence
    virtual double get_sigma_divergence() const = 0;
    // Get the wave vector in units of inverse angstroms
    virtual vec3<double> get_s0() const = 0;
    virtual vec3<double> get_unit_s0() const = 0;
    virtual vec3<double> get_polarization_normal() const = 0;
    virtual double get_polarization_fraction() const = 0;
    virtual double get_flux() const = 0;
    virtual double get_transmission() const = 0;
    virtual std::size_t get_num_scan_points() const = 0;
    virtual scitbx::af::shared<vec3<double> > get_s0_at_scan_points() const = 0;
    virtual vec3<double> get_s0_at_scan_point(std::size_t index) const = 0;
    virtual Probe get_probe() const = 0;
    virtual std::string get_probe_name() const = 0;

    virtual void set_direction(vec3<double> direction) = 0;
    virtual void set_wavelength(double wavelength) = 0;
    // Set the wave vector in units of inverse angstroms
    virtual void set_s0(vec3<double> s0) = 0;
    virtual void set_unit_s0(vec3<double> unit_s0) = 0;
    virtual void set_divergence(double divergence) = 0;
    // Set the standard deviation of the beam divergence
    virtual void set_sigma_divergence(double sigma_divergence) = 0;
    virtual void set_polarization_normal(vec3<double> polarization_normal) = 0;
    virtual void set_polarization_fraction(double polarization_fraction) = 0;
    virtual void set_flux(double flux) = 0;
    virtual void set_transmission(double transmission) = 0;
    virtual void set_s0_at_scan_points(
      const scitbx::af::const_ref<vec3<double> > &s0) = 0;
    virtual void set_probe(Probe probe) = 0;
    virtual void set_sample_to_source_distance(double sample_to_source_distance) = 0;

    virtual void reset_scan_points() = 0;
    virtual bool is_similar_to(const BeamBase &rhs,
                               double wavelength_tolerance,
                               double direction_tolerance,
                               double polarization_normal_tolerance,
                               double polarization_fraction_tolerance,
                               double divergence_tolerance,
                               double sigma_divergence_tolerance,
                               double flux_tolerance,
                               double transmission_tolerance,
                               double sample_to_source_distance_tolerance) const = 0;
    virtual void rotate_around_origin(vec3<double> axis, double angle) = 0;
    virtual bool operator!=(const BeamBase &rhs) const = 0;
    virtual bool operator==(const BeamBase &rhs) const = 0;
  };

  /** A class to represent a simple beam. */
  class Beam : public BeamBase {
  public:
    Beam()
        : wavelength_(0.0),
          direction_(0.0, 0.0, 1.0),
          divergence_(0.0),
          sigma_divergence_(0.0),
          polarization_normal_(0.0, 1.0, 0.0),
          polarization_fraction_(0.999),
          flux_(0),
          transmission_(1.0),
          probe_(Probe::xray),
          sample_to_source_distance_(0.) {}

    /**
     * @param s0 The incident beam vector.
     */
    Beam(vec3<double> s0)
        : divergence_(0.0),
          sigma_divergence_(0.0),
          polarization_normal_(0.0, 1.0, 0.0),
          polarization_fraction_(0.999),
          flux_(0),
          transmission_(1.0),
          probe_(Probe::xray),
          sample_to_source_distance_(0.) {
      DXTBX_ASSERT(s0.length() > 0);
      wavelength_ = 1.0 / s0.length();
      direction_ = -s0.normalize();
    }

    /**
     * @param direction The beam direction vector from sample to source
     * @param wavelength The wavelength of the beam
     */
    Beam(vec3<double> direction, double wavelength)
        : wavelength_(wavelength),
          divergence_(0.0),
          sigma_divergence_(0.0),
          polarization_normal_(0.0, 1.0, 0.0),
          polarization_fraction_(0.999),
          flux_(0),
          transmission_(1.0),
          probe_(Probe::xray),
          sample_to_source_distance_(0.) {
      DXTBX_ASSERT(direction.length() > 0);
      direction_ = direction.normalize();
    }

    /**
     * @param s0 The incident beam vector.
     * @param divergence The beam divergence
     * @param sigma_divergence The standard deviation of the beam divergence
     */
    Beam(vec3<double> s0, double divergence, double sigma_divergence)
        : divergence_(divergence),
          sigma_divergence_(sigma_divergence),
          polarization_normal_(0.0, 1.0, 0.0),
          polarization_fraction_(0.999),
          flux_(0),
          transmission_(1.0),
          probe_(Probe::xray),
          sample_to_source_distance_(0.) {
      DXTBX_ASSERT(s0.length() > 0);
      wavelength_ = 1.0 / s0.length();
      direction_ = -s0.normalize();
    }

    /**
     * @param direction The beam direction vector from sample to source
     * @param wavelength The wavelength of the beam
     * @param divergence The beam divergence
     * @param sigma_divergence The standard deviation of the beam divergence
     */
    Beam(vec3<double> direction,
         double wavelength,
         double divergence,
         double sigma_divergence)
        : wavelength_(wavelength),
          divergence_(divergence),
          sigma_divergence_(sigma_divergence),
          polarization_normal_(0.0, 1.0, 0.0),
          polarization_fraction_(0.999),
          flux_(0),
          transmission_(1.0),
          probe_(Probe::xray),
          sample_to_source_distance_(0.) {
      DXTBX_ASSERT(direction.length() > 0);
      direction_ = direction.normalize();
    }

    /**
     * @param direction The beam direction vector from sample to source
     * @param wavelength The wavelength of the beam
     * @param divergence The beam divergence
     * @param sigma_divergence The standard deviation of the beam divergence
     * @param polarization_normal The polarization plane
     * @param polarization_fraction The polarization fraction
     * @param flux The beam flux
     * @param transmission The beam transmission
     * @param probe The probe value
     */
    Beam(vec3<double> direction,
         double wavelength,
         double divergence,
         double sigma_divergence,
         vec3<double> polarization_normal,
         double polarization_fraction,
         double flux,
         double transmission,
         Probe probe)
        : wavelength_(wavelength),
          divergence_(divergence),
          sigma_divergence_(sigma_divergence),
          polarization_normal_(polarization_normal),
          polarization_fraction_(polarization_fraction),
          flux_(flux),
          transmission_(transmission),
          probe_(probe),
          sample_to_source_distance_(0.) {
      DXTBX_ASSERT(direction.length() > 0);
      direction_ = direction.normalize();
    }

    /**
     * @param direction The beam direction vector from source to sample
     * @param wavelength The wavelength of the beam
     * @param divergence The beam divergence
     * @param sigma_divergence The standard deviation of the beam divergence
     * @param polarization_normal The polarization plane
     * @param polarization_fraction The polarization fraction
     * @param flux The beam flux
     * @param transmission The beam transmission
     * @param probe The probe value
     * @param sample_to_source_distance (mm)
     */
    Beam(vec3<double> direction,
         double wavelength,
         double divergence,
         double sigma_divergence,
         vec3<double> polarization_normal,
         double polarization_fraction,
         double flux,
         double transmission,
         Probe probe,
         double sample_to_source_distance)
        : wavelength_(wavelength),
          divergence_(divergence),
          sigma_divergence_(sigma_divergence),
          polarization_normal_(polarization_normal),
          polarization_fraction_(polarization_fraction),
          flux_(flux),
          transmission_(transmission),
          probe_(probe),
          sample_to_source_distance_(sample_to_source_distance) {
      DXTBX_ASSERT(direction.length() > 0);
      direction_ = direction.normalize();
    }

    virtual ~Beam() {}

    vec3<double> get_sample_to_source_direction() const {
      return direction_;
    }

    virtual double get_wavelength() const {
      return wavelength_;
    }

    double get_divergence() const {
      return divergence_;
    }

    /** Get the standard deviation of the beam divergence */
    double get_sigma_divergence() const {
      return sigma_divergence_;
    }

    /** Set the sample to source direction. */
    void set_direction(vec3<double> direction) {
      DXTBX_ASSERT(direction.length() > 0);
      direction_ = direction.normalize();
    }

    virtual void set_wavelength(double wavelength) {
      wavelength_ = wavelength;
    }

    virtual vec3<double> get_s0() const {
      DXTBX_ASSERT(wavelength_ != 0.0);
      return -direction_ * 1.0 / wavelength_;
    }

    virtual void set_s0(vec3<double> s0) {
      DXTBX_ASSERT(s0.length() > 0);
      direction_ = -s0.normalize();
      wavelength_ = 1.0 / s0.length();
    }

    vec3<double> get_unit_s0() const {
      return -direction_;
    }

    void set_unit_s0(vec3<double> unit_s0) {
      DXTBX_ASSERT(unit_s0.length() > 0);
      direction_ = -(unit_s0.normalize());
    }

    void set_divergence(double divergence) {
      divergence_ = divergence;
    }

    /** Set the standard deviation of the beam divergence */
    void set_sigma_divergence(double sigma_divergence) {
      sigma_divergence_ = sigma_divergence;
    }

    vec3<double> get_polarization_normal() const {
      return polarization_normal_;
    }

    double get_polarization_fraction() const {
      return polarization_fraction_;
    }

    void set_polarization_normal(vec3<double> polarization_normal) {
      polarization_normal_ = polarization_normal;
    }

    void set_polarization_fraction(double polarization_fraction) {
      polarization_fraction_ = polarization_fraction;
    }

    void set_flux(double flux) {
      flux_ = flux;
    }

    void set_transmission(double transmission) {
      transmission_ = transmission;
    }

    double get_flux() const {
      return flux_;
    }

    double get_transmission() const {
      return transmission_;
    }

    std::size_t get_num_scan_points() const {
      return s0_at_scan_points_.size();
    }

    void set_s0_at_scan_points(const scitbx::af::const_ref<vec3<double> > &s0) {
      s0_at_scan_points_ = scitbx::af::shared<vec3<double> >(s0.begin(), s0.end());
    }

    scitbx::af::shared<vec3<double> > get_s0_at_scan_points() const {
      return s0_at_scan_points_;
    }

    vec3<double> get_s0_at_scan_point(std::size_t index) const {
      DXTBX_ASSERT(index < s0_at_scan_points_.size());
      return s0_at_scan_points_[index];
    }

    Probe get_probe() const {
      return probe_;
    }

    std::string get_probe_name() const {
      // Return a name that matches NeXus definitions from
      // https://manual.nexusformat.org/classes/base_classes/NXsource.html
      switch (probe_) {
      case xray:
        return std::string("x-ray");
      case electron:
        return std::string("electron");
      case neutron:
        return std::string("neutron");
      default:
        throw DXTBX_ERROR("Unknown probe type");
      }
    }

    static Probe get_probe_from_name(const std::string probe) {
      // Return a Probe matched to NeXus definitions from
      // https://manual.nexusformat.org/classes/base_classes/NXsource.html

      if (probe == "x-ray") {
        return Probe::xray;
      } else if (probe == "electron") {
        return Probe::electron;
      } else if (probe == "neutron") {
        return Probe::neutron;
      }

      throw DXTBX_ERROR("Unknown probe " + probe);
    }

    void set_probe(Probe probe) {
      probe_ = probe;
    }

    void reset_scan_points() {
      s0_at_scan_points_.clear();
    }

    /* Distance from sample to source in mm */
    double get_sample_to_source_distance() const {
      return sample_to_source_distance_;
    }

    /* Distance from sample to source in mm */
    void set_sample_to_source_distance(double sample_to_source_distance) {
      DXTBX_ASSERT(sample_to_source_distance >= 0.);
      sample_to_source_distance_ = sample_to_source_distance;
    }

    virtual bool operator==(const BeamBase &rhs) const {
      double eps = 1.0e-6;

      // scan-varying model checks
      if (get_num_scan_points() > 0) {
        if (get_num_scan_points() != rhs.get_num_scan_points()) {
          return false;
        }
        for (std::size_t j = 0; j < get_num_scan_points(); ++j) {
          vec3<double> this_s0 = get_s0_at_scan_point(j);
          vec3<double> other_s0 = rhs.get_s0_at_scan_point(j);
          double d_s0 = 0.0;
          for (std::size_t i = 0; i < 3; ++i) {
            d_s0 += std::abs(this_s0[i] - other_s0[i]);
          }
          if (d_s0 > eps) {
            return false;
          }
        }
      }

      // static model checks
      return std::abs(angle_safe(direction_, rhs.get_sample_to_source_direction()))
               <= eps
             && std::abs(wavelength_ - rhs.get_wavelength()) <= eps
             && std::abs(divergence_ - rhs.get_divergence()) <= eps
             && std::abs(sigma_divergence_ - rhs.get_sigma_divergence()) <= eps
             && std::abs(
                  angle_safe(polarization_normal_, rhs.get_polarization_normal()))
                  <= eps
             && std::abs(polarization_fraction_ - rhs.get_polarization_fraction())
                  <= eps
             && std::abs(flux_ - rhs.get_flux()) <= eps
             && std::abs(transmission_ - rhs.get_transmission()) <= eps
             && std::abs(sample_to_source_distance_
                         - rhs.get_sample_to_source_distance())
                  <= eps
             && (probe_ == rhs.get_probe());
    }

    virtual bool is_similar_to(const BeamBase &rhs,
                               double wavelength_tolerance,
                               double direction_tolerance,
                               double polarization_normal_tolerance,
                               double polarization_fraction_tolerance) const {
      // scan varying model checks
      if (get_num_scan_points() != rhs.get_num_scan_points()) {
        return false;
      }
      for (std::size_t i = 0; i < get_num_scan_points(); ++i) {
        vec3<double> s0_a = get_s0_at_scan_point(i);
        vec3<double> s0_b = rhs.get_s0_at_scan_point(i);

        vec3<double> us0_a = s0_a.normalize();
        vec3<double> us0_b = s0_b.normalize();
        if (std::abs(angle_safe(us0_a, us0_b)) > direction_tolerance) {
          return false;
        }

        double wavelength_a = 1.0 / s0_a.length();
        double wavelength_b = 1.0 / s0_b.length();
        if (std::abs(wavelength_a - wavelength_b) > wavelength_tolerance) {
          return false;
        }
      }

      // static model checks
      return std::abs(angle_safe(direction_, rhs.get_sample_to_source_direction()))
               <= direction_tolerance
             && std::abs(wavelength_ - rhs.get_wavelength()) <= wavelength_tolerance
             && std::abs(
                  angle_safe(polarization_normal_, rhs.get_polarization_normal()))
                  <= polarization_normal_tolerance
             && std::abs(polarization_fraction_ - rhs.get_polarization_fraction())
                  <= polarization_fraction_tolerance
             && (probe_ == rhs.get_probe());
    }

    virtual bool is_similar_to(const BeamBase &rhs,
                               double wavelength_tolerance,
                               double direction_tolerance,
                               double polarization_normal_tolerance,
                               double polarization_fraction_tolerance,
                               double divergence_tolerance = 1e-6,
                               double sigma_divergence_tolerance = 1e-6,
                               double flux_tolerance = 1e-6,
                               double transmission_tolerance = 1e-6,
                               double sample_to_source_tolerance = 1e-6) const {
      // scan varying model checks
      if (get_num_scan_points() != rhs.get_num_scan_points()) {
        return false;
      }
      for (std::size_t i = 0; i < get_num_scan_points(); ++i) {
        vec3<double> s0_a = get_s0_at_scan_point(i);
        vec3<double> s0_b = rhs.get_s0_at_scan_point(i);

        vec3<double> us0_a = s0_a.normalize();
        vec3<double> us0_b = s0_b.normalize();
        if (std::abs(angle_safe(us0_a, us0_b)) > direction_tolerance) {
          return false;
        }

        double wavelength_a = 1.0 / s0_a.length();
        double wavelength_b = 1.0 / s0_b.length();
        if (std::abs(wavelength_a - wavelength_b) > wavelength_tolerance) {
          return false;
        }
      }

      // static model checks
      return std::abs(angle_safe(direction_, rhs.get_sample_to_source_direction()))
               <= direction_tolerance
             && std::abs(wavelength_ - rhs.get_wavelength()) <= wavelength_tolerance
             && std::abs(
                  angle_safe(polarization_normal_, rhs.get_polarization_normal()))
                  <= polarization_normal_tolerance
             && std::abs(polarization_fraction_ - rhs.get_polarization_fraction())
                  <= polarization_fraction_tolerance
             && std::abs(divergence_ - rhs.get_divergence()) <= divergence_tolerance
             && std::abs(sigma_divergence_ - rhs.get_sigma_divergence())
                  <= sigma_divergence_tolerance
             && std::abs(flux_ - rhs.get_flux()) <= flux_tolerance
             && std::abs(transmission_ - rhs.get_transmission())
                  <= transmission_tolerance
             && std::abs(sample_to_source_distance_
                         - rhs.get_sample_to_source_distance())
                  <= sample_to_source_tolerance;
    }

    bool operator!=(const BeamBase &rhs) const {
      return !(*this == rhs);
    }

    void rotate_around_origin(vec3<double> axis, double angle) {
      direction_ = direction_.rotate_around_origin(axis, angle);
      polarization_normal_ = polarization_normal_.rotate_around_origin(axis, angle);
    }

    friend std::ostream &operator<<(std::ostream &os, const Beam &b);

  protected:
    vec3<double> direction_;
    double divergence_;
    double sigma_divergence_;
    vec3<double> polarization_normal_;
    double polarization_fraction_;
    double flux_;
    double transmission_;
    Probe probe_;
    double sample_to_source_distance_;

  private:
    double wavelength_;
    scitbx::af::shared<vec3<double> > s0_at_scan_points_;
  };

  /** Print beam information */
  inline std::ostream &operator<<(std::ostream &os, const Beam &b) {
    os << "Beam:\n";
    os << "    probe: " << b.get_probe_name() << "\n";
    os << "    wavelength: " << b.get_wavelength() << "\n";
    os << "    sample to source direction : "
       << b.get_sample_to_source_direction().const_ref() << "\n";
    os << "    divergence: " << b.get_divergence() << "\n";
    os << "    sigma divergence: " << b.get_sigma_divergence() << "\n";
    os << "    polarization normal: " << b.get_polarization_normal().const_ref()
       << "\n";
    os << "    polarization fraction: " << b.get_polarization_fraction() << "\n";
    os << "    flux: " << b.get_flux() << "\n";
    os << "    transmission: " << b.get_transmission() << "\n";
    os << "    sample to source distance: " << b.get_sample_to_source_distance()
       << "\n";
    return os;
  }
  class PolychromaticBeam : public Beam {
  public:
    PolychromaticBeam() {
      set_direction(vec3<double>(0.0, 0.0, 1.0));
      set_divergence(0.0);
      set_sigma_divergence(0.0);
      set_polarization_normal(vec3<double>(0.0, 1.0, 0.0));
      set_polarization_fraction(0.5);
      set_flux(0);
      set_transmission(1.0);
      set_probe(Probe::xray);
      set_sample_to_source_distance(0.0);
    }

    /**
     * @param direction The beam direction pointing sample to source
     */
    PolychromaticBeam(vec3<double> direction) {
      DXTBX_ASSERT(direction.length() > 0);
      direction_ = direction.normalize();
      set_divergence(0.0);
      set_sigma_divergence(0.0);
      set_polarization_normal(vec3<double>(0.0, 1.0, 0.0));
      set_polarization_fraction(0.5);
      set_flux(0);
      set_transmission(1.0);
      set_probe(Probe::xray);
      set_sample_to_source_distance(0.0);
    }

    /**
     * @param direction The beam direction pointing source to sample
     * @param sample_to_source_distance (mm)
     */
    PolychromaticBeam(vec3<double> direction, double sample_to_source_distance) {
      DXTBX_ASSERT(direction.length() > 0);
      direction_ = direction.normalize();
      set_sample_to_source_distance(sample_to_source_distance);
      set_divergence(0.0);
      set_sigma_divergence(0.0);
      set_polarization_normal(vec3<double>(0.0, 1.0, 0.0));
      set_polarization_fraction(0.999);
      set_flux(0);
      set_transmission(1.0);
      set_probe(Probe::xray);
    }

    /**
     * @param direction The beam direction pointing sample to source
     * @param divergence The beam divergence
     * @param sigma_divergence The standard deviation of the beam divergence
     */
    PolychromaticBeam(vec3<double> direction,
                      double divergence,
                      double sigma_divergence) {
      DXTBX_ASSERT(direction.length() > 0);
      direction_ = direction.normalize();
      set_divergence(divergence);
      set_sigma_divergence(sigma_divergence);
      set_polarization_normal(vec3<double>(0.0, 1.0, 0.0));
      set_polarization_fraction(0.5);
      set_flux(0);
      set_transmission(1.0);
      set_probe(Probe::xray);
      set_sample_to_source_distance(0.0);
    }

    /**
     * @param direction The beam direction pointing sample to source
     * @param divergence The beam divergence
     * @param sigma_divergence The standard deviation of the beam divergence
     * @param polarization_normal The polarization plane
     * @param polarization_fraction The polarization fraction
     * @param flux The beam flux
     * @param transmission The beam transmission
     * @param probe The probe value
     */
    PolychromaticBeam(vec3<double> direction,
                      double divergence,
                      double sigma_divergence,
                      vec3<double> polarization_normal,
                      double polarization_fraction,
                      double flux,
                      double transmission,
                      Probe probe) {
      DXTBX_ASSERT(direction.length() > 0);
      direction_ = direction.normalize();
      set_divergence(divergence);
      set_sigma_divergence(sigma_divergence);
      set_polarization_normal(polarization_normal);
      set_polarization_fraction(polarization_fraction);
      set_flux(flux);
      set_transmission(transmission);
      set_probe(probe);
      set_sample_to_source_distance(0.0);
    }

    /**
     * @param direction The beam direction pointing source to sample
     * @param divergence The beam divergence
     * @param sigma_divergence The standard deviation of the beam divergence
     * @param polarization_normal The polarization plane
     * @param polarization_fraction The polarization fraction
     * @param flux The beam flux
     * @param transmission The beam transmission
     * @param probe The probe value
     * @param sample_to_source_distance (mm)
     */
    PolychromaticBeam(vec3<double> direction,
                      double divergence,
                      double sigma_divergence,
                      vec3<double> polarization_normal,
                      double polarization_fraction,
                      double flux,
                      double transmission,
                      Probe probe,
                      double sample_to_source_distance) {
      DXTBX_ASSERT(direction.length() > 0);
      direction_ = direction.normalize();
      set_divergence(divergence);
      set_sigma_divergence(sigma_divergence);
      set_polarization_normal(polarization_normal);
      set_polarization_fraction(polarization_fraction);
      set_flux(flux);
      set_transmission(transmission);
      set_probe(probe);
      set_sample_to_source_distance(sample_to_source_distance);
    }

    double get_wavelength() const {
      throw DXTBX_ERROR("PolychromaticBeam has no fixed wavelength");
      return -1.;
    }

    void set_wavelength(double wavelength) {
      throw DXTBX_ERROR("PolychromaticBeam has no fixed wavelength");
    }

    vec3<double> get_s0() const {
      throw DXTBX_ERROR("PolychromaticBeam has no fixed s0");
      return vec3<double>(0., 0., 0.);
    }

    void set_s0(vec3<double> s0) {
      throw DXTBX_ERROR("PolychromaticBeam has no fixed s0");
    }

    std::size_t get_num_scan_points() const {
      throw DXTBX_ERROR("PolychromaticBeam has no fixed s0");
      return 1;
    }

    void set_s0_at_scan_points(const scitbx::af::const_ref<vec3<double> > &s0) {
      throw DXTBX_ERROR("PolychromaticBeam has no fixed s0");
    }

    scitbx::af::shared<vec3<double> > get_s0_at_scan_points() const {
      throw DXTBX_ERROR("PolychromaticBeam has no fixed s0");
      return scitbx::af::shared<vec3<double> >(1, (0., 0., 0.));
    }

    vec3<double> get_s0_at_scan_point(std::size_t index) const {
      throw DXTBX_ERROR("PolychromaticBeam has no fixed s0");
      return vec3<double>(0., 0., 0.);
    }

    void reset_scan_points() {
      throw DXTBX_ERROR("PolychromaticBeam has no fixed s0");
    }

    bool operator==(const BeamBase &rhs) const {
      double eps = 1.0e-6;

      return std::abs(angle_safe(direction_, rhs.get_sample_to_source_direction()))
               <= eps
             && std::abs(divergence_ - rhs.get_divergence()) <= eps
             && std::abs(sigma_divergence_ - rhs.get_sigma_divergence()) <= eps
             && std::abs(
                  angle_safe(polarization_normal_, rhs.get_polarization_normal()))
                  <= eps
             && std::abs(polarization_fraction_ - rhs.get_polarization_fraction())
                  <= eps
             && std::abs(flux_ - rhs.get_flux()) <= eps
             && std::abs(transmission_ - rhs.get_transmission()) <= eps
             && std::abs(sample_to_source_distance_
                         - rhs.get_sample_to_source_distance())
                  <= eps
             && (probe_ == rhs.get_probe());
    }

    bool is_similar_to(const BeamBase &rhs,
                       double wavelength_tolerance,
                       double direction_tolerance,
                       double polarization_normal_tolerance,
                       double polarization_fraction_tolerance) const {
      return is_similar_to(rhs,
                           direction_tolerance,
                           polarization_normal_tolerance,
                           polarization_fraction_tolerance);
    }

    bool is_similar_to(const BeamBase &rhs,
                       double direction_tolerance,
                       double polarization_normal_tolerance,
                       double polarization_fraction_tolerance,
                       double divergence_tolerance = 1e-6,
                       double sigma_divergence_tolerance = 1e-6,
                       double flux_tolerance = 1e-6,
                       double transmission_tolerance = 1e-6,
                       double sample_to_source_tolerance = 1e-6) const {
      return std::abs(angle_safe(direction_, rhs.get_sample_to_source_direction()))
               <= direction_tolerance
             && std::abs(
                  angle_safe(polarization_normal_, rhs.get_polarization_normal()))
                  <= polarization_normal_tolerance
             && std::abs(polarization_fraction_ - rhs.get_polarization_fraction())
                  <= polarization_fraction_tolerance
             && std::abs(flux_ - rhs.get_flux()) <= flux_tolerance
             && std::abs(transmission_ - rhs.get_transmission())
                  <= transmission_tolerance
             && std::abs(sample_to_source_distance_
                         - rhs.get_sample_to_source_distance())
                  <= sample_to_source_tolerance
             && (probe_ == rhs.get_probe());
    }
  };

  /** Print beam information */
  inline std::ostream &operator<<(std::ostream &os, const PolychromaticBeam &b) {
    os << "Beam:\n";
    os << "    probe: " << b.get_probe_name() << "\n";
    os << "    sample to source direction : "
       << b.get_sample_to_source_direction().const_ref() << "\n";
    os << "    divergence: " << b.get_divergence() << "\n";
    os << "    sigma divergence: " << b.get_sigma_divergence() << "\n";
    os << "    polarization normal: " << b.get_polarization_normal().const_ref()
       << "\n";
    os << "    polarization fraction: " << b.get_polarization_fraction() << "\n";
    os << "    flux: " << b.get_flux() << "\n";
    os << "    transmission: " << b.get_transmission() << "\n";
    os << "    sample to source distance : " << b.get_sample_to_source_distance()
       << "\n";
    return os;
  }
}}  // namespace dxtbx::model

#endif  // DXTBX_MODEL_BEAM_H
