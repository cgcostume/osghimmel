Using _osghimmel_ is supposed to be as simple as possible without interfering with your scenegraph structure.



# Himmel Time #

`TimeF` manages an `osg::Timer` and features an interface for floating time in the closed interval `[`0;1`]` representing a full day and standard c time (`time_t`) simultaneously. The time updates have to be requested explicitly, thus simplifying usage between multiple recipients.

Each instance of `AbstractHimmel` requires a `TimeF` instance.

```
TimeF *timef = new TimeF(0.f, 60.f); // one minute for cycling a day
himmel->assignTime(timef);

...
timef->update();

```

The time starts cycling automatically, but can also be paused, stopped, or set to a specific value. Seconds per Cycle specify the duration of one day in seconds and is used while running. It can be changed at any time without tampering the time.




# Texture Mapped Himmel #

`AbstractMappedHimmel` features textured skies with support for essentially four common texture projections:

| **Cube** Mapping | ![http://osghimmel.googlecode.com/svn/wiki/figures/example_cubemap_small.png](http://osghimmel.googlecode.com/svn/wiki/figures/example_cubemap_small.png) | Features the best detail so far, but cube edges can be spotted. |
|:-----------------|:----------------------------------------------------------------------------------------------------------------------------------------------------------|:----------------------------------------------------------------|
| **Paraboloid** Mapping | ![http://osghimmel.googlecode.com/svn/wiki/figures/example_paraboloidmap_small.png](http://osghimmel.googlecode.com/svn/wiki/figures/example_paraboloidmap_small.png) | Typically utilizes 360 photographs where the full horizon is mapped into one picture (checkout Fisheye lenses). Features good information density distribution. |
| **Polar** Mapping | ![http://osghimmel.googlecode.com/svn/wiki/figures/example_polarmap_small.png](http://osghimmel.googlecode.com/svn/wiki/figures/example_polarmap_small.png) | Best free resources available for, but provide an unnecessary amount of detail towards the zenith (due to projection) - still my favorites. Can be applied in half (`MM_Half`) or full (`MM_Full`) mode. |
| **Sphere** Mapping | ![http://osghimmel.googlecode.com/svn/wiki/figures/example_spheremap_small.png](http://osghimmel.googlecode.com/svn/wiki/figures/example_spheremap_small.png) | Similar to Paraboloid Mapping, but instead of a hemisphere, a full sphere is mapped to the texture. The outer hemisphere is usually less detailed than the inner one. Due to floating precision, and inaccurate sampling an erroneous (singularity like) point arises opposite of the inner hemisphere. The map can be oriented towards an axis (currently `MM_TowardsNegY` and `MM_TowardsPosZ` are supported). |

The `TimeF` given by superclass `AbstractHimmel` is used only for texture transitions. An additional `TimeF` object is used internally for the timed rotation around the zenith.

## Texture Transitions ##

When assigning textures to an `AbstractMappedHimmel` instance, a float time has to be specified, that is used for internal keypoints. Keypoints define point in times where one texture gets replaced (abruptly or by smooth fading) by the following one. The transition duration can be specified in floating time and should be smaller than the minimum time distance between two adjacent keypoints.

![http://osghimmel.googlecode.com/svn/wiki/figures/texture_transitions.png](http://osghimmel.googlecode.com/svn/wiki/figures/texture_transitions.png)

Code Fragment:

```
osg::ref_ptr<PolarMappedHimmel> himmel = new PolarMappedHimmel(PolarMappedHimmel::MM_Half);

himmel->assignTime(timef);
himmel->setTransitionDuration(0.4f);

himmel->getOrCreateTexture2D(0)->setImage(osgDB::readImageFile("resources/polar_half_art_1.jpg"));
himmel->getOrCreateTexture2D(1)->setImage(osgDB::readImageFile("resources/polar_half_art_2.jpg"));

himmel->pushTextureUnit(0, 0.0f);
himmel->pushTextureUnit(1, 0.5f);

...
```


## Horizon Band ##

Some texture projections (polar mapping with `MM_Half` and paraboloid mapping) discard the lower hemisphere, resulting in a hard edge to gl clear color. To avoid this, these himmels offer a gradient called `HorizonBand`. This is given by scale, width, and offset as well as a color and a bottom color. The scale is in `[`0;1`[`, with 1.0 overlapping the skies vertical 180 degrees. Width specifies the vertical region uniformly filled with color. Currently smoothstep blending is used for the gradient, but inv\_squared interpolation yields good results too.

![http://osghimmel.googlecode.com/svn/wiki/figures/horizon_band.png](http://osghimmel.googlecode.com/svn/wiki/figures/horizon_band.png)

Code Fragment:

```
// The second parameter activates the horizon band.
osg::ref_ptr<PolarMappedHimmel> himmel = new PolarMappedHimmel(PolarMappedHimmel::MM_Half, true);

himmel->assignTime(timef);

// Values do not exactly correspond to figure.

himmel->hBand()->setBottomColor(osg::Vec4(0.50f, 0.50f, 0.50f, 1.00f));
himmel->hBand()->setColor(      osg::Vec4(0.74f, 0.80f, 0.83f, 1.00f));

himmel->hBand()->setOffset(0.1f);
himmel->hBand()->setScale( 0.7f);
himmel->hBand()->setWidth( 0.2f);

...
```


## Rotation around Zenith ##

Rotation around Zenith (RaZ), if used discreetly, can greatly influence the dynamic of a scene, by slightly rotating the given texture mapped himmel around the up vector. It accesses its own `TimeF` object and thereby remains independent from the himmels timef, which in case of the `AbstractMappedHimmel` is used for texture transitions. The speed of the rotation can be specified in seconds per RaZ, and the direction by an enum (`RD_NorthEastSouthWest` and `RD_NorthWestSouthEast` were used to avoid misinterpretations of CW and CCW).

Code Fragment:

```
osg::ref_ptr<PolarMappedHimmel> himmel = new PolarMappedHimmel(PolarMappedHimmel::MM_Half);

himmel->assignTime(timef);

// reasonable values should be around 2000+
himmel->setSecondsPerRAZ(2000.f);
himmel->setRazDirection(AbstractMappedHimmel::RD_NorthWestSouthEast);

...
```