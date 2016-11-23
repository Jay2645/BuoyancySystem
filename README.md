# BuoyancySystem
A system for buoyancy and boat physics in Unreal Engine 4.

There are a few main parts to this plugin:

* **BuoyantComponent**: This is an ActorComponent which can be added to anything which can float. This uses spheres to test for buoyancy -- faster than calculating how much of the object is underwater, but also less accurate and requires the user to manually specify where the spheres should be placed and how large they should be. There are options for setting the Displacement Ratio (how much force is applied upwards to provide buoyancy) and the Mass Multiplier (how much the force of gravity is multiplied by when the BuoyantComponent is falling downwards). You can also set points which will cause the object to lose buoyancy if any of the points goes underwater -- this can model, for example, the top of a boat going a little too low, causing water to rush in and sink the boat.

* **BuoyancyManager**: This is in charge of actually managing and creating the waves in the water. It uses overlapping clusters of Gerstner Waves to generate a realistic ocean. There is a MaterialParameterCollection which goes along with the BuoyancyManager; if this collection is set, then any defaults set up in the BuoyancyManager will be propagated to the MaterialParameterCollection. Keep in mind that this class is intended to be a child of the GameState class.

* **BoatVehicle**: This is a class inheriting from Unreal's `WheeledVehicle` class. The wheels on this boat are intended to be invisible, and the suspension has a very large drop value allowing the "wheels" to fall to the ocean floor to do their work. There is a very narrow base between the wheels, which causes a larger amount of tilt when the boat takes corners. There are also classes inheriting from the various `Wheel` classes and the like to support the `BoatVehicle` class. The `BoatVehicle` also has a `BuoyancyComponent` to allow it to move up and down with the waves. The `BoatVehicle` also has a "stencil" mesh, which is the section of the boat which (ideally) shouldn't have any water in it (i.e., the inside, where the people sit). This mesh is written to UE4's stencil buffer, which is fancy-talk for saying that it'll be drawn on top of the water at all times (unless the boat is sinking).

* **ShipSystem**: This is intended for any extra parts which fit on the boat. These can either be visual parts, parts which add extra functionality to the ship, etc. The only class currently implemented that inherits from `ShipSystemComponent` is the `EngineSystem`, which provides appropriate engine noises when the ship is throttled up or down.

# Content

In addition to the code, there is also a little bit of content included with this project -- namely, 3 water materials, some Material Functions, a Material Parameter Collection, and a lava material.

To view them, go to your Content Browser, select "View Options" (in the bottom right), and ensure that "Show Plugin Content" is checked. A whole *bunch* of stuff will appear in the Sources Panel to the right (click the little button with a few lines and an arrow in the upper-left of the content browser if you don't see it). What you're looking for is "BuoyancySystem Content". Scroll down in the list until you find it. You should see a folder called `Materials`, and 2 subfolders -- `Functions` and `Water`. Functions contains all the material functions, while `Water` contains the actual water materials. The base `Materials` folder contains the MaterialParameterCollection.

The material M_MasterWater is the "main" water material, and the other materials are Material Instances of it. The MaterialParameterCollection WaterParameters is the MaterialParameterCollection that the `BuoyancyManager` should reference. You can adjust it as need be to fit whatever look you're going for for you water, and those should propagate properly to your `BuoyancyManager`.

#Installation

First, make a `Plugins` folder at your project root (where the .uproject file is), if you haven't already. Then, clone this project into a subfolder in your Plugins directory. After that, open up your project's .uproject file in Notepad (or a similar text editor), and change the `"AdditionalDependencies"` and `"Plugins"` sections to look like this:

```
	"Modules": [
		{
			"Name": "YourProjectName",
			"Type": "Runtime",
			"LoadingPhase": "Default",
			"AdditionalDependencies": [
				<OTHER DEPENDENCIES GO HERE>
				"BuoyancySystem"
			]
		}
	],
	"Plugins": [
		<OTHER PLUGINS GO HERE>
		{
			"Name": "BuoyancySystem",
			"Enabled": true
		}
	]
```

You can now open up your project in Unreal. You might be told that your project is out of date, and the editor will ask if you want to rebuild them. You do. After that, open up the Plugins menu, scroll down to the bottom, and ensure that the "BuoyancySystem" plugin is enabled.

In order to enable the stencil buffer and have the interior render properly, you need to enable the Custom Stencil Buffer. Go to ***Window > Project Settings > Rendering > Post Process > Custom Depth-Stencil Pass*** and set it to **Enabled with Stencil**. This will allow the interior of your boat to be drawn "on top of" the water, which creates the illusion that the water plane isn't actually intersecting the inside of your boat.

A side effect of this is that the interior of your boat needs to be a separate mesh from the outside -- if the entire boat is written to the stencil buffer, it'll always be drawn on top of the waves, and if the stencil buffer isn't used at all, the interior of your boat will flood whenever the water bobs up and down.

After that, you're done! You can subclass `ABoatVehicle` in C++ or Blueprints and tweak it to your heart's desire, until you have your very own boat!
