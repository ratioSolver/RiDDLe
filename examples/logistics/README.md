# Logistics

These examples show an application of planning to logistics.

In this domain, specifically, there are different **cities** :cityscape:, each having different **locations** :building_construction:, some of which are **airports** :flight_departure:.

Some **packages** :package: are initially placed in some locations. The goal is to move these packages to other desired locations.

In order to move the packages, some **trucks** :truck: are available, initially in their respective locations. So, in order to move a package from one location to another, a truck must *travel* to the package location, *load* it, move to the desired location, and *unload* it.
Trucks, however, can only travel between locations within the same city. In order to move a package from one city to another, the trucks must transport the packages to the city airport and unload them. At the airport, packages can be *loaded* onto a **plane** :airplane:, *transported* to the airport of another city and *unloaded*, so that they can be loaded onto the trucks of the destination city and moved to the desired location.
