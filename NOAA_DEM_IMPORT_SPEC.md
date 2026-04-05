# NOAA DEM Import - Design Specification

## 📋 Overview

**Feature:** Import real-world elevation data from NOAA and other geographic data sources, enabling game developers to create levels based on actual locations.

**Problem Solved:** Creating realistic terrain manually is time-consuming and often results in "fake-looking" landscapes. Real-world DEMs (Digital Elevation Models) provide:
- Actual geographic data
- Realistic geomorphology
- Unique, recognizable locations
- Educational value
- Time savings

**Solution:** Import system for multiple DEM formats with geographic metadata handling, coordinate transformations, and game-engine scaling.

---

## 🎯 Goals

1. **Real-World Accuracy** - Preserve actual elevation data
2. **Multiple Data Sources** - Support NOAA, USGS, NASA, etc.
3. **Format Flexibility** - GeoTIFF, ASCII Grid, HGT, USGS DEM
4. **Easy Integration** - Simple import workflow
5. **Game-Ready Output** - Auto-scale for game engines

---

## 🌍 Data Sources

### Primary Sources

#### 1. NOAA (National Oceanic and Atmospheric Administration)
- **Coverage:** USA coastal and bathymetric data
- **Resolution:** 1/3 arc-second to 1 arc-second (~10m to 30m)
- **Format:** GeoTIFF, ASCII Grid
- **URL:** https://www.ngdc.noaa.gov/mgg/coastal/
- **Use Case:** Coastal terrain, underwater topography

#### 2. USGS 3DEP (3D Elevation Program)
- **Coverage:** USA complete coverage
- **Resolution:** 1m to 30m (varies by location)
- **Format:** GeoTIFF, Cloud Optimized GeoTIFF
- **URL:** https://www.usgs.gov/3d-elevation-program
- **Use Case:** High-resolution USA terrain
- **Quality:** Excellent! Gold standard for USA data

#### 3. NASA SRTM (Shuttle Radar Topography Mission)
- **Coverage:** Global (60°N to 56°S)
- **Resolution:** 1 arc-second (~30m) and 3 arc-second (~90m)
- **Format:** HGT (height) files
- **URL:** https://www2.jpl.nasa.gov/srtm/
- **Use Case:** Global terrain data
- **Quality:** Good for mountains, valleys, major features

#### 4. ASTER GDEM (Advanced Spaceborne Thermal Emission)
- **Coverage:** Global (83°N to 83°S)
- **Resolution:** 1 arc-second (~30m)
- **Format:** GeoTIFF
- **URL:** https://asterweb.jpl.nasa.gov/gdem.asp
- **Use Case:** Global coverage where SRTM unavailable
- **Quality:** Good, some artifacts in flat areas

#### 5. OpenTopography
- **Coverage:** Research-grade topographic data
- **Resolution:** Sub-meter LiDAR available
- **Format:** GeoTIFF, ASCII Grid, LAS (point clouds)
- **URL:** https://www.opentopography.org/
- **Use Case:** Ultra-high-resolution research data
- **Quality:** Excellent! Best available data

### Secondary Sources

#### 6. JAXA AW3D30
- **Coverage:** Global
- **Resolution:** 30m
- **Format:** GeoTIFF
- **Use Case:** Alternative to SRTM

#### 7. EU-DEM
- **Coverage:** Europe
- **Resolution:** 25m
- **Format:** GeoTIFF
- **Use Case:** European terrain

#### 8. Local Government Data
- **Coverage:** City/county level
- **Resolution:** 1m to 5m (very high!)
- **Format:** LiDAR, GeoTIFF
- **Use Case:** Urban environments, detailed local terrain

---

## 📁 Supported File Formats

### 1. GeoTIFF (Primary Format)
**Priority:** HIGH  
**Complexity:** HIGH (requires LibTIFF or GDAL)

**Characteristics:**
- TIFF image with geospatial metadata
- Elevation stored as pixel values
- Coordinate system embedded
- Bounding box information
- 16-bit or 32-bit elevation data

**Metadata Tags:**
- `ModelTiepointTag` - Ties pixels to coordinates
- `ModelPixelScaleTag` - Pixel size in map units
- `GeoKeyDirectoryTag` - Coordinate system info
- `GeoDoubleParamsTag` - Coordinate parameters

**Implementation:**
```cpp
struct GeoTIFFMetadata {
    double boundingBox[4];  // [minX, minY, maxX, maxY]
    double pixelSize[2];    // [xSize, ySize] in map units
    int coordinateSystem;   // EPSG code (e.g., 4326 = WGS84)
    std::string projection; // "WGS 84", "UTM Zone 11N", etc.
    double noDataValue;     // Value representing missing data
    std::string elevationUnits; // "meters", "feet"
};

class GeoTIFFImporter {
public:
    static ImportResult importGeoTIFF(const std::string& filePath);
    
private:
    static GeoTIFFMetadata readMetadata(TIFF* tif);
    static Image readElevationBand(TIFF* tif, int band = 0);
    static void handleNoData(Image& heightmap, double noDataValue);
    static void normalizeElevation(Image& heightmap, double minElev, double maxElev);
};
```

**Libraries:**
- **Option A:** LibTIFF + LibGeoTIFF (lightweight, TIFF-specific)
- **Option B:** GDAL (comprehensive, handles everything, larger)

**Recommendation:** Start with LibTIFF for simplicity, add GDAL later for advanced features.

---

### 2. ASCII Grid (.asc, ESRI Grid Format)
**Priority:** MEDIUM  
**Complexity:** LOW (text-based, easy to parse)

**Format:**
```
ncols         4800
nrows         6000
xllcorner     -120.0
yllcorner     40.0
cellsize      0.000833333333333
NODATA_value  -9999
 125.5 126.2 127.1 128.0 ...
 124.8 125.5 126.3 ...
 ...
```

**Implementation:**
```cpp
struct ASCIIGridHeader {
    int ncols;
    int nrows;
    double xllcorner;  // Lower-left corner X (longitude)
    double yllcorner;  // Lower-left corner Y (latitude)
    double cellsize;   // Cell size in degrees
    double nodata;     // NODATA value
};

class ASCIIGridImporter {
public:
    static ImportResult importASCIIGrid(const std::string& filePath);
    
private:
    static ASCIIGridHeader parseHeader(std::ifstream& file);
    static Image readGrid(std::ifstream& file, const ASCIIGridHeader& header);
    static void handleNoData(Image& heightmap, double nodata);
};
```

**Advantages:**
- Simple text format
- Easy to parse
- No dependencies
- Human-readable

**Disadvantages:**
- Large file size
- Slower to read
- Limited metadata

---

### 3. HGT (SRTM Height Format)
**Priority:** MEDIUM  
**Complexity:** LOW (binary, simple structure)

**Format:**
- Binary file, big-endian
- 16-bit signed integers
- Elevation in meters
- Fixed resolution (1201x1201 or 3601x3601)
- Filename encodes lat/lon: `N37W122.hgt` = 37°N, 122°W

**Implementation:**
```cpp
struct HGTMetadata {
    int latitude;   // From filename: N37 = 37, S37 = -37
    int longitude;  // From filename: W122 = -122, E122 = 122
    int resolution; // 1201 or 3601 (1" or 3" arc-second)
};

class HGTImporter {
public:
    static ImportResult importHGT(const std::string& filePath);
    
private:
    static HGTMetadata parseFilename(const std::string& filename);
    static Image readHeightData(std::ifstream& file, int resolution);
    static void swapEndian(std::vector<int16_t>& data); // HGT is big-endian
};
```

**Example Filenames:**
- `N37W122.hgt` - San Francisco area
- `N27E086.hgt` - Mount Everest area
- `S33E151.hgt` - Sydney area

---

### 4. USGS DEM (.dem)
**Priority:** LOW (legacy format)  
**Complexity:** HIGH (complex header structure)

**Characteristics:**
- Legacy USGS format
- Complex ASCII header
- Multiple elevation profiles
- Less common now (superseded by GeoTIFF)

**Implementation:** Optional, low priority (most USGS data now available as GeoTIFF).

---

## 🔧 Implementation Architecture

### DEM Import Dialog

```
┌─────────────────── Import DEM Data ───────────────────┐
│                                                        │
│ File: [Browse...] C:\DEMs\GrandCanyon.tif             │
│                                                        │
│ ┌──── Detected Information ────────────────────────┐  │
│ │ Format: GeoTIFF                                  │  │
│ │ Resolution: 3601 x 3601 pixels                   │  │
│ │ Coordinate System: WGS 84 (EPSG:4326)            │  │
│ │ Bounding Box:                                    │  │
│ │   Lat: 36.0° N to 37.0° N                       │  │
│ │   Lon: 113.0° W to 112.0° W                     │  │
│ │ Elevation Range: 686m to 2569m                   │  │
│ │ Location: Grand Canyon, Arizona                  │  │
│ │ Cell Size: ~30m                                  │  │
│ └──────────────────────────────────────────────────┘  │
│                                                        │
│ ┌──── Import Options ───────────────────────────────┐ │
│ │ Target Resolution: [1024 ▼] pixels                │ │
│ │   (Current: 3601 → Downsample to 1024)           │ │
│ │                                                   │ │
│ │ [✓] Preserve Aspect Ratio                        │ │
│ │ [✓] Auto-Normalize Elevation to [0.0, 1.0]       │ │
│ │                                                   │ │
│ │ Sea Level: [0] meters                            │ │
│ │   (Values below this → 0.0)                      │ │
│ │                                                   │ │
│ │ [ ] Crop to Region of Interest                   │ │
│ │     (Select area after import)                   │ │
│ │                                                   │ │
│ │ [ ] Clamp Extreme Elevations                     │ │
│ │     Min: [____] m   Max: [____] m                │ │
│ └───────────────────────────────────────────────────┘ │
│                                                        │
│ ┌──── Engine Integration ───────────────────────────┐ │
│ │ Target Engine: [Unreal Engine ▼]                  │ │
│ │ Recommended World Size: 4000m (auto-calculated)   │ │
│ │                                                   │ │
│ │ Real-world Extent: 111km x 111km                  │ │
│ │ Scaled to Game: 4km x 4km (1:27.75 scale)        │ │
│ │                                                   │ │
│ │ Height Range: 686m - 2569m (1883m variation)     │ │
│ │ Scaled Height: 0m - 68m (UE5-appropriate)        │ │
│ └───────────────────────────────────────────────────┘ │
│                                                        │
│ ┌──── Preview ──────────────────────────────────────┐ │
│ │ [Grayscale heightmap preview - 256x256]          │ │
│ │ Min: 0.0 (dark) ───── Max: 1.0 (light)           │ │
│ └───────────────────────────────────────────────────┘ │
│                                                        │
│                  [Cancel]  [Import DEM]                │
└────────────────────────────────────────────────────────┘
```

---

## 🗺️ Geographic Metadata Handling

### Coordinate Systems

**Common Systems:**
- **WGS 84 (EPSG:4326)** - Latitude/Longitude (most common)
- **UTM (Universal Transverse Mercator)** - Metric coordinates by zone
- **State Plane** - USA state-specific systems
- **Web Mercator (EPSG:3857)** - Web mapping

**Implementation:**
```cpp
struct GeographicMetadata {
    // Coordinate system
    int epsgCode;               // e.g., 4326 = WGS84
    std::string projection;     // "WGS 84", "UTM Zone 11N"
    
    // Bounding box (in native coordinate system)
    double minX, minY, maxX, maxY;
    
    // Elevation data
    double minElevation;        // meters
    double maxElevation;        // meters
    std::string elevationUnits; // "meters", "feet"
    double noDataValue;         // e.g., -9999
    
    // Resolution
    double pixelSizeX;          // meters or degrees
    double pixelSizeY;
    int width, height;          // pixels
    
    // Optional: Location info
    std::string locationName;   // "Grand Canyon, AZ"
    std::string dataSource;     // "USGS 3DEP"
    std::string acquisitionDate; // "2023-04-15"
};

class GeographicMetadataHandler {
public:
    static GeographicMetadata extractMetadata(const std::string& filePath, DEM_Format format);
    static std::string getLocationName(double lat, double lon); // Reverse geocode
    static double calculateRealWorldExtent(const GeographicMetadata& meta); // km
    static void displayMetadataInfo(const GeographicMetadata& meta); // UI
};
```

---

## 🎮 Game Engine Integration

### Scale Calculation

**Challenge:** Real-world DEMs are often HUGE:
- Grand Canyon: 111km x 111km real-world
- Game level: Maybe 4km x 4km
- Scale ratio: 1:27.75

**Solution:** Automatic scaling with metadata preservation.

```cpp
struct ScaleConversion {
    double realWorldWidth;      // km
    double realWorldHeight;     // km
    double gameWorldWidth;      // meters (e.g., 4000m = 4km)
    double gameWorldHeight;     // meters
    double scaleRatio;          // e.g., 1:27.75
    
    double realMinElev;         // meters (e.g., 686m)
    double realMaxElev;         // meters (e.g., 2569m)
    double gameMinElev;         // meters (e.g., 0m)
    double gameMaxElev;         // meters (e.g., 68m)
    double elevScaleRatio;      // e.g., 1:27.75 (match horizontal)
};

class DEMScaler {
public:
    static ScaleConversion calculateScaling(
        const GeographicMetadata& meta,
        TargetEngine engine,
        float targetWorldSize  // meters
    );
    
    static Image scaleForGameEngine(
        const Image& demData,
        const ScaleConversion& scaling
    );
    
    static void displayScalingInfo(const ScaleConversion& scaling); // UI
};
```

### Example Scaling

**Input (Grand Canyon DEM):**
- Real extent: 111km x 111km
- Real elevation: 686m to 2569m (1883m variation)
- Resolution: 3601x3601 pixels

**Output (Unreal Engine 4km world):**
- Game extent: 4km x 4km
- Game elevation: 0m to 68m (68m variation)
- Resolution: 1024x1024 pixels (downsampled)
- Scale ratio: 1:27.75 (both horizontal and vertical)

**Why this works:**
- Preserves terrain shape (same scale ratio horizontal/vertical)
- Appropriate for UE5 character (180cm)
- 68m variation = climbable mountains in-game
- Recognizable as Grand Canyon when playing

---

## 🔄 Import Workflow

### Step-by-Step Process

**1. File Selection**
```
User: File → Import DEM Data (Ctrl+Shift+O)
System: Show file browser filtered to .tif, .asc, .hgt, .dem
User: Select file
```

**2. Format Detection**
```cpp
DEM_Format detectFormat(const std::string& filePath) {
    std::string ext = getExtension(filePath);
    if (ext == ".tif" || ext == ".tiff") return DEM_Format::GeoTIFF;
    if (ext == ".asc") return DEM_Format::ASCIIGrid;
    if (ext == ".hgt") return DEM_Format::HGT;
    if (ext == ".dem") return DEM_Format::USGS_DEM;
    return DEM_Format::Unknown;
}
```

**3. Metadata Extraction**
```
System: Read file metadata
  - Coordinate system
  - Bounding box
  - Elevation range
  - Resolution
  - Location (if available)
System: Display in Import Dialog
```

**4. User Configuration**
```
User: Set target resolution (1024)
User: Choose "Auto-Normalize" ✓
User: Set sea level (0m)
User: Select target engine (Unreal)
System: Calculate recommended world size
System: Show scaling preview
```

**5. Import Processing**
```cpp
ImportResult importDEM(const DEMImportConfig& config) {
    // 1. Load raw elevation data
    Image rawDEM = loadDEMFile(config.filePath, config.format);
    
    // 2. Handle NODATA values
    handleNoData(rawDEM, config.metadata.noDataValue);
    
    // 3. Apply sea level
    applySeaLevel(rawDEM, config.seaLevel);
    
    // 4. Clamp extremes (if enabled)
    if (config.clampExtremes) {
        clampElevation(rawDEM, config.minClamp, config.maxClamp);
    }
    
    // 5. Downsample to target resolution
    Image resampled = resampleImage(rawDEM, config.targetRes, config.targetRes);
    
    // 6. Normalize to [0.0, 1.0]
    if (config.autoNormalize) {
        normalize(resampled);
    }
    
    // 7. Calculate scaling metadata
    ScaleConversion scaling = calculateScaling(
        config.metadata,
        config.targetEngine,
        config.worldSize
    );
    
    return ImportResult{
        .success = true,
        .heightmap = resampled,
        .metadata = config.metadata,
        .scaling = scaling
    };
}
```

**6. Integration**
```
System: Replace current heightmap
System: Clear undo/redo
System: Update display
System: Show success message with scaling info
User: Start editing!
```

---

## 🧪 Test Cases

### Test Case 1: Import SRTM Tile
**File:** `N37W122.hgt` (San Francisco Bay Area)  
**Expected:**
- Detects HGT format
- Parses filename: 37°N, 122°W
- Resolution: 3601x3601 (1" arc-second)
- Elevation range: 0m (sea level) to ~800m (peaks)
- Downsamples to 1024x1024
- Normalizes to [0.0, 1.0]
- Preserves San Francisco Bay (water areas at 0.0)

### Test Case 2: Import USGS GeoTIFF
**File:** `GrandCanyon_DEM.tif`  
**Expected:**
- Detects GeoTIFF format
- Reads geospatial metadata (WGS84, bounding box)
- Elevation range: ~686m to ~2569m
- Handles NODATA values (outside coverage area)
- Scales to appropriate game size
- Recognizable canyon features preserved

### Test Case 3: Import ASCII Grid
**File:** `MountEverest.asc`  
**Expected:**
- Parses ASCII header correctly
- Handles large elevation values (8000m+)
- Manages NODATA (-9999) properly
- Normalizes extreme elevations appropriately

### Test Case 4: Coastal DEM with Bathymetry
**File:** `CoastalRegion.tif` (includes underwater data)  
**Expected:**
- Negative elevations (underwater) handled
- Sea level set to 0m
- Underwater terrain: -50m to 0m → [0.0, 0.25]
- Land terrain: 0m to 150m → [0.25, 1.0]
- Smooth beach transitions

---

## 🚀 Implementation Phases

### Phase 1: GeoTIFF Support (8-10 hours)
- [ ] Integrate LibTIFF library
- [ ] Create GeoTIFFImporter class
- [ ] Extract elevation band
- [ ] Read geospatial metadata
- [ ] Handle NODATA values
- [ ] Basic normalization
- [ ] Test with USGS data

### Phase 2: Import Dialog (4-5 hours)
- [ ] Create DEM Import dialog UI
- [ ] File browser integration
- [ ] Metadata display
- [ ] Import options (resolution, normalize, etc.)
- [ ] Preview window
- [ ] Wire up to GeoTIFFImporter

### Phase 3: Scaling System (3-4 hours)
- [ ] GeographicMetadata structure
- [ ] ScaleConversion calculator
- [ ] Engine integration (suggest world size)
- [ ] Display scaling info in UI
- [ ] Test with various DEMs

### Phase 4: ASCII Grid Support (2-3 hours)
- [ ] ASCIIGridImporter class
- [ ] Header parser
- [ ] Grid reader
- [ ] NODATA handling
- [ ] Test with .asc files

### Phase 5: HGT Support (2-3 hours)
- [ ] HGTImporter class
- [ ] Filename parser (lat/lon extraction)
- [ ] Binary reader
- [ ] Endian swap (big-endian → little-endian)
- [ ] Test with SRTM tiles

### Phase 6: Polish & Documentation (2-3 hours)
- [ ] Error handling
- [ ] Progress indicators (large files)
- [ ] User documentation
- [ ] Example workflow guide
- [ ] Test with real NOAA data

**Total Estimated Time:** 21-28 hours

---

## 📚 Libraries & Dependencies

### Required
- **LibTIFF** - For GeoTIFF reading
  - License: BSD-like (permissive)
  - Size: ~500KB compiled
  - Complexity: Medium
  - Alternative: GDAL (much larger, more features)

### Optional
- **LibGeoTIFF** - For geospatial metadata
  - License: Public Domain
  - Size: ~100KB compiled
  - Enhances LibTIFF with geo tags

- **GDAL (Geospatial Data Abstraction Library)** - For advanced features
  - License: MIT/X (permissive)
  - Size: ~20MB compiled (large!)
  - Supports ALL geo formats
  - Coordinate transformations
  - Future enhancement

### Alternative: stb-style Single-Header Libraries
- **tinytiff** - Minimal TIFF reader (if LibTIFF too complex)
- Custom GeoTIFF parser (basic functionality)

---

## 🌟 Use Cases

### Use Case 1: Hiking Game - Local Trails
**Scenario:** Developer wants to create hiking game of their local mountain trails.

**Workflow:**
1. Download local DEM from USGS 3DEP (1m resolution)
2. Import DEM (Ctrl+Shift+O)
3. Select region containing trails
4. Scale to 2km x 2km game world
5. Paint trail markers on terrain
6. Add trees, rocks, signs
7. Export to Unity
8. Result: Realistic hiking simulator of actual location!

### Use Case 2: Historical Battle Recreation
**Scenario:** Developer creating historical battle game (e.g., Gettysburg).

**Workflow:**
1. Download Gettysburg DEM from USGS
2. Import with 1863 sea level
3. Scale to match historical maps
4. Paint roads, fortifications
5. Result: Accurate battlefield terrain!

### Use Case 3: Fantasy Game - Epic Landscapes
**Scenario:** Fantasy RPG with recognizable real-world locations.

**Workflow:**
1. Import Mount Everest DEM
2. Import Grand Canyon DEM
3. Blend terrain in painting
4. Add fantasy elements
5. Result: Familiar yet fantastical world!

### Use Case 4: Flight Simulator
**Scenario:** Flight sim over real terrain.

**Workflow:**
1. Import multiple SRTM tiles (cover large area)
2. Stitch tiles together
3. Maintain real-world scale
4. Add airports, landmarks
5. Export as terrain mesh
6. Result: Fly over actual geography!

---

## 🎓 User Education

### DEM Import Guide
Create `DEM_IMPORT_GUIDE.md`:
- Where to download DEM data
- Understanding DEM formats
- Choosing resolution
- Scaling for game engines
- Example imports (Grand Canyon, etc.)
- Troubleshooting

### Video Tutorial Ideas
- "Import Real-World Terrain in 5 Minutes"
- "Create Grand Canyon Level for Unreal Engine"
- "Using NOAA Data for Coastal Levels"

---

## 🔮 Future Enhancements

### Phase 2: Online DEM Browser
- [ ] Integrated map view
- [ ] Click to select region
- [ ] Download directly from USGS/NOAA
- [ ] No manual file download needed

### Phase 3: Multi-Tile Stitching
- [ ] Load multiple adjacent DEM tiles
- [ ] Auto-stitch into seamless terrain
- [ ] Handle tile boundaries smoothly
- [ ] Cover large areas (100km+)

### Phase 4: Bathymetry Integration
- [ ] NOAA bathymetric charts
- [ ] Underwater terrain detail
- [ ] Coral reefs, trenches
- [ ] Submarine gameplay support

### Phase 5: Vegetation Data
- [ ] USGS NLCD (National Land Cover Database)
- [ ] Auto-place trees based on land cover
- [ ] Grasslands, forests, urban areas
- [ ] Biome classification from GIS data

---

## ✅ Success Metrics

**How do we know this feature is successful?**
- ✅ Users import real-world DEMs regularly
- ✅ Forum posts: "Made my hometown in Unreal!"
- ✅ Example projects shared (Grand Canyon, etc.)
- ✅ Educational use (geography students)
- ✅ Professional use (simulation, training)

---

## 🚨 Challenges & Solutions

### Challenge 1: File Size
**Problem:** GeoTIFFs can be gigabytes  
**Solution:** 
- Stream reading (don't load entire file)
- Progressive downsampling
- Crop to region of interest

### Challenge 2: Coordinate Transformations
**Problem:** Different coordinate systems  
**Solution:**
- Start with WGS84 only (most common)
- Add PROJ library later for transformations
- Warn users if uncommon system

### Challenge 3: NODATA Handling
**Problem:** Missing data, voids, water areas  
**Solution:**
- Detect NODATA value from metadata
- Replace with 0.0 or interpolate
- Show warning if significant voids

### Challenge 4: Extreme Elevations
**Problem:** Mount Everest (8848m) hard to normalize  
**Solution:**
- Clamp extreme option
- Scale to appropriate game range
- Show original elevation in metadata

---

## 📚 Related Documentation

- [TODO.md](TODO.md) - Phase 2.5 task breakdown
- [ImageImporter.h](ImageImporter.h) - Existing import system (extend for DEMs)
- [PHASE_2_1_IMPORT_COMPLETE.md](PHASE_2_1_IMPORT_COMPLETE.md) - RAW import documentation

---

**Status:** 📋 Design Complete, Ready for Implementation  
**Priority:** Medium-High - Unique feature, significant value  
**Estimated Effort:** 21-28 hours  
**Target Version:** V0.9.0 or V1.0  
**Dependencies:** LibTIFF (or GDAL)
